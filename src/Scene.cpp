#include "Scene.h"
#include "Utility.h"

#include <utility>
#include <cmath>
#include <random>
#include <iostream>
#include <omp.h>

Scene::Scene(Colour colour,
             Camera* camera,
             std::vector<std::shared_ptr<Shape>> objects,
             std::vector<std::shared_ptr<LightSource>> lightSources,
             int nBounces,
             RenderMode renderMode)
    : backgroundColour(std::move(colour)),
      camera(camera),
      shapes(std::move(objects)),
      lightSources(std::move(lightSources)),
      nBounces(nBounces),
      renderMode(renderMode) {
  buildBspTree();
}

Scene::Scene(JsonObject json) : nBounces(json["nbounces"].asInt()), camera(Camera::fromJson(json["camera"].asObject())) {
  std::string renderModeString = json["rendermode"].asString();
  if (renderModeString == "binary") {
    renderMode = BINARY;
  } else if (renderModeString == "phong") {
    renderMode = PHONG;
  } else if (renderModeString == "pathtracer") {
    renderMode = PATHTRACER;
  } else {
    throw std::runtime_error("Invalid render mode");
  }
  JsonObject cameraObject = json["camera"].asObject();


  JsonObject sceneObject = json["scene"].asObject();
  backgroundColour = Colour(sceneObject["backgroundcolor"].asArray());
  JsonArray lightsJson = sceneObject["lightsources"].asArray();
  for (const auto &i : lightsJson) {
    lightSources.push_back(LightSource::fromJson(i.asObject()));
  }
  JsonArray objectsJson = sceneObject["shapes"].asArray();
  for (const auto &i : objectsJson) {
    shapes.push_back(Shape::fromJson(i.asObject()));
  }
  buildBspTree();
}

Scene Scene::loadFromFile(const std::string &filename) {
  JsonParser parser;
  return Scene(parser.parseFile(filename).asObject());
}

Image Scene::render() {
  if (renderMode == BINARY) {
    return renderBinary();
  } else if (renderMode == PHONG) {
    return renderBlinnPhong();
  } else if (renderMode == PATHTRACER) {
    return renderPathtracer();
  } else {
    throw std::runtime_error("Invalid render mode");
  }
}

Image Scene::renderBinary() {
  Image image(camera->getWidth(), camera->getHeight());

  for (int y = 0; y < camera->getHeight(); y++) {
    for (int x = 0; x < camera->getWidth(); x++) {
      Ray ray = camera->getRay(x, y, 0.5, 0.5);
      std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

      Colour pixelColour;
      if (intersection.has_value()) {
        pixelColour = Colour(1, 0, 0);
      } else {
        pixelColour = Colour(0, 0, 0);
      }

      image.setColor(x, y, pixelColour);
    }
  }
  return image;
}

Image Scene::renderShaded() {
  Image image(camera->getWidth(), camera->getHeight());

  auto colourSampler = [this](const Scene &s, const Ray &r) { return this->sampleShaded(r); };

  for (int y = 0; y < camera->getHeight(); y++) {
    printProgress(y, camera->getHeight());
    for (int x = 0; x < camera->getWidth(); x++) {
      image.setColor(x, y, sample(x, y, 8, colourSampler));
    }
  }
  printProgress(camera->getHeight(), camera->getHeight());
  return image;
}

Image Scene::renderBlinnPhong() {
  Image image(camera->getWidth(), camera->getHeight());

  auto colourSampler = [this](const Scene &s, const Ray &r) { return this->sampleBlinnPhong(r); };

  int done = 0;
  auto start = std::chrono::high_resolution_clock::now();

  printProgress(0, camera->getHeight());

#pragma omp parallel for num_threads(8) schedule(dynamic) default(none) shared(image, colourSampler, start, done)
  for (int y = 0; y < camera->getHeight(); y++) {
    for (int x = 0; x < camera->getWidth(); x++) {
      image.setColor(x, y, sample(x, y, 25, colourSampler));
    }
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    printProgress(++done, camera->getHeight(), elapsed);
  }
  return image;
}

double getPixelVariance(Image &image, Image &varianceImage, int x, int y) {
  int neighborOffsets[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
  Colour currentPixelColor = image.getColour(x, y);
  Colour varianceColour = varianceImage.getColour(x, y);
  double baseVariance = varianceColour.getX() + varianceColour.getY() + varianceColour.getZ();
  double neighbourVariance = 0;

  for (auto &offset : neighborOffsets) {
    int nx = x + offset[0];
    int ny = y + offset[1];

    // Check if neighbor is within the image bounds
    if (nx >= 0 && nx < image.getWidth() && ny >= 0 && ny < image.getHeight()) {
      Colour neighborColor = image.getColour(nx, ny);
      neighbourVariance += (currentPixelColor - neighborColor).magnitudeSquared(); // or any other difference measure
    }
  }

  return baseVariance + (neighbourVariance / 8.0);
}

Image Scene::renderPathtracer() {
  Image image(camera->getWidth(), camera->getHeight());
  Image varianceImage(camera->getWidth(), camera->getHeight());

  auto colourSampler = [this](const Scene &s, const Ray &r) { return this->samplePathtracer(r); };

  int baselineSamples = 5;
  int additionalSamplesMax = 50;

  int done = 0;
  auto start = std::chrono::high_resolution_clock::now();

  printProgress(0, camera->getHeight());

#pragma omp parallel for num_threads(8) schedule(dynamic) default(none) shared(image, colourSampler, start, done, baselineSamples, varianceImage)
  for (int y = 0; y < camera->getHeight(); y++) {
    for (int x = 0; x < camera->getWidth(); x++) {
      Colour pixelColour;
      std::vector<Colour> samples;
      double scale = 1.0 / baselineSamples;
      for (int i = 0; i < baselineSamples; i++) {
        double xOffset = random_double();
        double yOffset = random_double();
        Ray ray = camera->getRay(x, y, xOffset, yOffset);
        auto colour = samplePathtracer(ray);
        pixelColour += colour * scale;
        samples.push_back(colour);
      }
      Colour pixelVariance;
      for (int i = 0; i < baselineSamples; i++) {
        Colour difference = Colour(samples[i] - pixelColour);
        pixelVariance += Colour(difference * difference) * scale;
      }
      image.setColor(x, y, pixelColour);
      varianceImage.setColor(x, y, pixelVariance);
    }
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    printProgress(++done, camera->getHeight(), elapsed);
  }

  int totalSamples = 0;
  int maxSamples = (baselineSamples + additionalSamplesMax) * camera->getWidth() * camera->getHeight();
#pragma omp parallel for num_threads(8) schedule(dynamic) default(none) shared(image, colourSampler, start, done, baselineSamples, varianceImage, additionalSamplesMax, totalSamples)
  for (int y = 0; y < camera->getHeight(); y++) {
    for (int x = 0; x < camera->getWidth(); x++) {
      Colour pixelColour = image.getColour(x, y);
//      Colour pixelVariance = varianceImage.getColour(x, y);
      double variance = getPixelVariance(image, varianceImage, x, y);
      double additionalSamples = std::clamp(10 * std::pow(variance, 1.0 / 3.0), 0.0, 1.0);
      int nSamples = additionalSamples * additionalSamplesMax;
      totalSamples += nSamples + baselineSamples;
      double scale = 1.0 / (nSamples + baselineSamples);
      pixelColour = Colour(pixelColour * ((double)baselineSamples / (baselineSamples + nSamples)));
      for (int i = 0; i < nSamples; i++) {
          double xOffset = random_double();
          double yOffset = random_double();
          Ray ray = camera->getRay(x, y, xOffset, yOffset);
          auto colour = samplePathtracer(ray);
          pixelColour += colour * scale;
      }
      image.setColor(x, y, pixelColour);
    }
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    printProgress(++done, camera->getHeight(), elapsed);
  }
  std::cout << std::endl << "Total samples: " << totalSamples << " out of " << maxSamples << std::endl;

  return image;
}

Colour Scene::sample(int x,
                     int y,
                     int nSamples,
                     const std::function<Colour(const Scene &, const Ray &)> &sampleFunction) {
  Colour pixelColour;
  double scale = 1.0 / nSamples;
  for (int i = 0; i < nSamples; i++) {
    double xOffset = random_double();
    double yOffset = random_double();
    Ray ray = camera->getRay(x, y, xOffset, yOffset);
    pixelColour += sampleFunction(*this, ray) * scale;
  }
  return pixelColour;
}

Colour Scene::sampleShaded(const Ray &ray, int depth) {
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

  if (!intersection.has_value()) return backgroundColour;

  Vector3D normal = intersection.value().first->getSurfaceNormal(ray.at(intersection.value().second));

  // Taking absolute of negative values
  double r = std::fabs(normal.getX());
  double g = std::fabs(normal.getY());
  double b = std::fabs(normal.getZ());

  return Colour(r, g, b);
}

Colour Scene::sampleBlinnPhong(const Ray &ray, int depth) {
  // Base case: if we've reached the maximum number of bounces, return the background color
  if (depth >= nBounces) return backgroundColour;

  // Check if the ray intersects with any objects in the scene
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

  // If the ray doesn't intersect with anything, return the background color
  if (!intersection.has_value()) return backgroundColour;

  // Unpack the intersection data
  auto [hitShape, hitDistance] = intersection.value();
  Vector3D hitPoint = ray.at(hitDistance);
  Vector3D normal = hitShape->getSurfaceNormal(hitPoint).normalize();
  Material* material = hitShape->getMaterial();

  Colour baseDiffuse = material->getDiffuseColour();
  if (material->isTextured()) {
    Vector2D uv = hitShape->getUVCoordinates(hitPoint).value();
    baseDiffuse = material->getTexture()->getUVColour(uv);
  }

  // Initialize the colour with ambient light if defined, otherwise black
  Colour ambientIntensity = Colour(0.3, 0.3, 0.3);
  Colour ambientColour = Colour(ambientIntensity * baseDiffuse);

  Colour diffuseSpecularSum = Colour();

  // Diffuse and specular calculations for each light source
  for (const auto &lightSource : lightSources) {
    auto [lightDir, lightDistance] = lightSource->getDirectionAndDistance(hitPoint);
    Vector3D viewDir = (camera->getPosition() - hitPoint).normalize();
    Vector3D halfDir = (lightDir + viewDir).normalize();

    Colour lightIntensity = isInShadowPathtracer(
        Ray(hitPoint, lightDir),
        lightSource->getIntensity(),
        hitPoint + lightDir * lightDistance);

    if (lightIntensity.max() > 0) {
      // Diffuse term
      double ln = std::abs(lightDir.dot(normal));
      Colour id = Colour(lightIntensity * baseDiffuse);
      Colour diffuseColour = Colour(id * ln * material->getKd());
      diffuseSpecularSum += diffuseColour * material->getKd();

      // Blinn-Phong specular term
      double hn = std::abs(halfDir.dot(normal));
      Colour is = Colour(lightIntensity * material->getSpecularColour());
      Colour specularColour = Colour(is * std::pow(hn, material->getSpecularExponent()) * material->getKs());
      diffuseSpecularSum += specularColour * material->getKs() / (lightDistance * lightDistance);
    }
  }

  // Reflectivity and refraction
  Colour colour = Colour(ambientColour + diffuseSpecularSum);

  double reflectance = material->getIsReflective();
  if (material->getIsRefractive()) {
    // Calculate Schlick's approximation
    double cosTheta = std::abs(ray.direction.dot(normal));
    double R0 = std::pow((1 - material->getRefractiveIndex()) / (1 + material->getRefractiveIndex()), 2);
    reflectance = R0 + (1 - R0) * std::pow(1 - cosTheta, 5);
  }
  reflectance *= material->getReflectivity();

  colour = Colour(colour * (1 - reflectance));

  if (material->getIsReflective()) {
    Vector3D reflectDir = ray.direction.reflect(normal).normalize();
    if (material->getRoughness() > 0) {
      reflectDir = (reflectDir + normal.randomInHemisphere() * material->getRoughness()).normalize();
    }
    Ray reflectRay(hitPoint, reflectDir); // Avoid self-intersection
    Colour reflectSample = sampleBlinnPhong(reflectRay, depth + 1);
    colour += reflectSample * reflectance;
  }
  if (material->getIsRefractive()) {
    Vector3D refractDir = ray.direction.refract(normal, material->getRefractiveIndex());
    if (material->getRoughness() > 0) {
      refractDir = (refractDir + normal.randomInHemisphere() * material->getRoughness()).normalize();
    }
    Ray refractRay(hitPoint, refractDir);
    Colour refractSample = sampleBlinnPhong(refractRay, depth + 1);
    colour += refractSample * (1 - reflectance);
  }

  // Clamp colour values to ensure they are within the valid range
  colour = colour.clamp();

  return colour;
}

bool Scene::isInShadow(const Vector3D &point, const LightSource &light) const {
  auto [lightDir, distanceToLight] = light.getDirectionAndDistance(point);
  Ray shadowRay(point, lightDir);
  return isInShadow(shadowRay, distanceToLight, light);
}

bool Scene::isInShadow(const Ray &shadowRay, double maxDistance, const LightSource &light) const {
  Interval interval = Interval(0.0001, maxDistance);
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(shadowRay, interval);
  return intersection.has_value();
}

Colour Scene::isInShadowPathtracer(const Ray &shadowRay, Colour lightIntensity, const Vector3D& lightPosition, int depth) const {
  if (depth >= nBounces) return Colour(0, 0, 0);

  Interval interval = Interval(0.0001, (lightPosition - shadowRay.origin).magnitude());
  Colour result;

  auto intersection = checkIntersection(shadowRay, interval);
  if (!intersection.has_value()) {
    return lightIntensity;
  }

  auto [hitShape, hitDistance] = intersection.value();
  Vector3D hitPoint = shadowRay.at(hitDistance);
  Material* material = hitShape->getMaterial();

  if (material->getIsRefractive()) {
    // Calculate the refractive ray
    double refractiveIndex = material->getRefractiveIndex();
    Vector3D normal = hitShape->getSurfaceNormal(hitPoint);
    Vector3D refractDirection = shadowRay.direction.refract(normal, material->getRefractiveIndex());

    Vector3D lightDirection = (lightPosition - shadowRay.origin).normalize();

    // Calculate Schlick's approximation
    double cosTheta = std::abs(shadowRay.direction.dot(normal));
    double R0 = std::pow((1 - material->getRefractiveIndex()) / (1 + material->getRefractiveIndex()), 2);
    double reflectance = R0 + (1 - R0) * std::pow(1 - cosTheta, 5);

    Ray lightRay = Ray(hitPoint, lightDirection);
    Ray refractRay = Ray(hitPoint, refractDirection);
    Ray reflectRay = Ray(hitPoint, shadowRay.direction.reflect(normal));

    double refractFactor = 1 - reflectance;
    double reflectFactor = reflectance;
    double lightFactor = 1 - reflectance;  // std::abs(refractRay.direction.dot(lightRay.direction) * (1 - reflectance));

    Colour refractBaseIntensity = Colour(lightIntensity * refractFactor);
    Colour reflectBaseIntensity = Colour(lightIntensity * reflectFactor);

    if (!checkIntersection(lightRay, Interval(0.0001, (lightPosition - hitPoint).magnitude()))) {
      return Colour(lightIntensity * lightFactor);
    }
    Colour refractIntensity = isInShadowPathtracer(refractRay, refractBaseIntensity, lightPosition, depth + 1);

    return refractIntensity;
  } else {
    return Colour(0, 0, 0);
  }

}

Colour Scene::samplePathtracer(const Ray &ray, int depth) {
  // Base case: if we've reached the maximum number of bounces, return black
  if (depth >= nBounces) return Colour(0, 0, 0);

  // Check if the ray intersects with any objects in the scene
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

  // If the ray doesn't intersect with anything, return the background backgroundColour
  if (!intersection.has_value()) return backgroundColour;

  auto [hitShape, hitDistance] = intersection.value();

  Vector3D hitPoint = ray.at(hitDistance);
  Vector3D normal = hitShape->getSurfaceNormal(ray.at(intersection.value().second));
  Material* material = hitShape->getMaterial();
  MaterialType materialType = material->getType();

  Colour reflectColour, refractColour;
  Colour brdfColour;
  if (materialType == MaterialType::METAL) {
    Vector3D reflectDir;
    Vector3D perfectReflectDir = ray.direction.reflect(normal).normalize();
    if (material->getRoughness() > 0) {
      // Perturb the reflection direction for rough surfaces
      Vector3D roughReflectDir = (ray.direction.reflect(normal) + normal.randomInHemisphere() * material->getRoughness()).normalize();
      reflectDir = (perfectReflectDir + (roughReflectDir - perfectReflectDir) * material->getRoughness()).normalize();
    } else {
      // Perfect mirror reflection for smooth surfaces
      reflectDir = perfectReflectDir;
    }

    // Evaluate the BRDF for the final reflection direction
    Colour brdfContribution = material->evaluateBRDF(-ray.direction, reflectDir, normal, std::nullopt).clamp();

    // Cast a new ray in the reflection direction and accumulate its contribution
    Ray reflectRay(hitPoint, reflectDir);
    auto reflectSample = samplePathtracer(reflectRay, depth + 1);
    brdfColour = Colour(reflectSample * brdfContribution);
  } else if (materialType == MaterialType::GLASS) {
    double fresnelReflectance = (double) material->evaluateBRDF(-ray.direction, normal, normal, std::nullopt).getRed() / 255; // Assuming RGB components are the same
    double reflectProb = fresnelReflectance;
    double refractProb = 1.0 - fresnelReflectance;

    // Handle Reflection
    Colour result;
    if (reflectProb > 0.01) {
      Vector3D reflectDir = ray.direction.reflect(normal).normalize();
      Ray reflectRay(hitPoint, reflectDir);
      result += samplePathtracer(reflectRay, depth + 1) * reflectProb;
    }
    if (refractProb > 0.01) {
      Vector3D refractDir = ray.direction.refract(normal, material->getRefractiveIndex()).normalize();
      Ray refractRay(hitPoint, refractDir);
      result += samplePathtracer(refractRay, depth + 1) * refractProb;
    }
    brdfColour = result;
  } else {
    // Diffuse illumination

    // Get the diffuse factor and backgroundColour of the object
    double diffuseFactor = intersection.value().first->getMaterial()->getKd();
    Colour diffuseColour = intersection.value().first->getMaterial()->getDiffuseColour();
    std::optional<Vector2D> uv = std::nullopt;
    if (material->isTextured()) {
      uv = hitShape->getUVCoordinates(hitPoint).value();
        diffuseColour = material->getTexture()->getUVColour(uv.value());
    }

    // Direct illumination
    Colour directIllumination(0, 0, 0);
    for (const auto &lightSource : lightSources) {
      for (int i = 0; i < lightSource->samplingFactor; i++) {
        auto [toLight, distanceToLight] = lightSource->getDirectionAndDistance(hitPoint);
        Vector3D lightDirection = toLight.normalize();
        Vector3D outgoingDirection = -ray.direction;

        Colour lightIntensity = isInShadowPathtracer(
            Ray(hitPoint, toLight),
            lightSource->getIntensity(),
            hitPoint + (toLight * distanceToLight));

        if (lightIntensity.max() > 0) {
          Colour brdfContribution = hitShape->getMaterial()->evaluateBRDF(lightDirection, outgoingDirection, normal, uv);
          directIllumination += Colour(brdfContribution * lightIntensity) / (lightSource->samplingFactor * distanceToLight * distanceToLight);
        }
      }
    }

    // Russian Roulette termination
    if (depth > 3) {
      double terminationProbability = std::max(0.05, 1 - reflectColour.max());
      double randomValue = random_double();

      if (randomValue < terminationProbability) {
        return directIllumination;
      } else {
        directIllumination = Colour(directIllumination / (1 - terminationProbability));
      }
    }

    // Indirect illumination
    int nSamples = std::max(1, 10 - depth * 2);
    Colour indirectIllumination(0, 0, 0);
    for (int i = 0; i < nSamples; i++) {
      // Get a random direction in the hemisphere of the normal
      Vector3D bounceDirection = normal + normal.randomInHemisphere();  // * material->getKd();

      // Create a new ray starting at the intersection point and going in the bounce direction
      Ray bounceRay = Ray(ray.at(hitDistance), bounceDirection.normalize());

      auto newBounceColour = samplePathtracer(bounceRay, depth + 1);

      // Calculate the colour of the bounce ray
      indirectIllumination += newBounceColour;
    }
    indirectIllumination = Colour(indirectIllumination * diffuseFactor * diffuseColour / nSamples);
    brdfColour = Colour(directIllumination + indirectIllumination * diffuseFactor * diffuseColour);
  }
  return brdfColour;

//  // Reflectivity and refraction
//  double reflectivity = material->getIsReflective();
//  if (material->getIsRefractive()) {
//    // Calculate Schlick's approximation
//    double cosTheta = std::abs(ray.direction.dot(normal));
//    double R0 = std::pow((1 - material->getRefractiveIndex()) / (1 + material->getRefractiveIndex()), 2);
//    reflectivity = R0 + (1 - R0) * std::pow(1 - cosTheta, 5);
//  }
//  reflectivity *= material->getReflectivity();
//
//  Colour reflectColour;
//  if (material->getIsReflective() && reflectivity > 0) {
//    Vector3D reflectDir = ray.direction.reflect(normal).normalize();
//    if (material->getRoughness() > 0) {
//      reflectDir = (reflectDir + normal.randomInHemisphere() * material->getRoughness()).normalize();
//    }
//    Ray reflectRay(hitPoint, reflectDir); // Avoid self-intersection
//    auto reflectSample = samplePathtracer(reflectRay, depth + 1);
//    reflectColour = Colour(reflectSample * reflectivity);
//  }
//
//  Colour refractColour;
//  if (material->getIsRefractive() && reflectivity < 1) {
//    Vector3D refractDir = ray.direction.refract(normal, material->getRefractiveIndex());
//    if (material->getRoughness() > 0) {
//      refractDir = (refractDir + normal.randomInHemisphere() * material->getRoughness()).normalize();
//    }
//    Ray refractRay(hitPoint, refractDir);
//    auto refractSample = samplePathtracer(refractRay, depth + 1);
//    refractColour = Colour(refractSample * (1 - reflectivity));
//  }
}

std::optional<std::pair<std::shared_ptr<Shape>, double>> Scene::checkIntersection(const Ray &ray, Interval interval) const {
  return checkIntersectionNode(bspTree.get(), ray, interval);
}

std::optional<std::pair<std::shared_ptr<Shape>, double>> Scene::checkIntersectionNode(const Node* node, const Ray &ray, Interval interval) const {
  if (node == nullptr) {
    return std::nullopt;
  }

  if (!node->aabb.intersects(ray, interval)) {
    // Quick rejection if the ray doesn't intersect the node's AABB
    return std::nullopt;
  }

  std::shared_ptr<Shape> closestShape = nullptr;
  double closestT = std::numeric_limits<double>::infinity();

  // Check intersection with the current node's shape
  if (node->shape) {
    auto t = node->shape->checkIntersection(ray, interval);
    if (t && t.value() < closestT) {
      closestShape = node->shape;
      closestT = t.value();
      interval = Interval(interval.getMin(), closestT);
    }
  }

  // Determine which child nodes to visit
  // This involves checking the position of the ray relative to the node's splitting plane
  // Pseudo-code: if (ray intersects with left plane) { ... }

  // Recursively check intersections in the child nodes
  auto leftResult = checkIntersectionNode(node->left.get(), ray, interval);
  if (leftResult && leftResult.value().second < closestT) {
    closestShape = leftResult.value().first;
    closestT = leftResult.value().second;
    interval = Interval(interval.getMin(), closestT);
  }

  auto rightResult = checkIntersectionNode(node->right.get(), ray, interval);
  if (rightResult && rightResult.value().second < closestT) {
    closestShape = rightResult.value().first;
    closestT = rightResult.value().second;
  }

  if (closestShape) {
    return std::make_pair(closestShape, closestT);
  } else {
    return std::nullopt;
  }
}

// Print the progress as a percentage as a whole number
void Scene::printProgress(int current, int total, std::chrono::milliseconds elapsed) {
  int progress = (int) (100.0 * current / total);
  if (elapsed < std::chrono::milliseconds(0)) {
    std::cout << "\rRendering: " << progress << "% complete" << std::flush;
  } else {
    std::cout << "\rRendering: " << progress << "% complete, " << elapsed.count() << "ms elapsed" << std::flush;
  }
}

void Scene::addShape(const std::shared_ptr<Shape>& shape) {
  shapes.push_back(shape);
  buildBspTree();
}

void Scene::buildBspTree() {
  bspTree = buildBspTree(shapes.begin(), shapes.end());
}

std::unique_ptr<Node> Scene::buildBspTree(std::vector<std::shared_ptr<Shape>>::iterator begin,
                                          std::vector<std::shared_ptr<Shape>>::iterator end) {
  if (begin == end) {
    return nullptr;
  }

  // Calculate the AABB for the entire set of shapes
  AABB totalAABB = (*begin)->getAABB();
  for (auto it = begin + 1; it != end; ++it) {
    totalAABB = totalAABB.encapsulate((*it)->getAABB());
  }

  // Initialize SAH variables
  double bestCost = std::numeric_limits<double>::infinity();
  auto bestSplit = begin;
  int bestAxis = -1;

  // Constants for SAH cost calculation
  const double C_trav = 1.0; // Cost of traversing a node
  const double C_inter = 1.0; // Cost of an intersection test

  // Iterate over each axis
  for (int axis = 0; axis < 3; ++axis) {
    // Sort shapes based on the centroid of their AABBs along the current axis
    std::sort(begin, end, [axis](const auto& a, const auto& b) {
      return a->getAABB().getCentroid()[axis] < b->getAABB().getCentroid()[axis];
    });

    // Calculate SAH cost for each split position
    for (auto mid = begin + 1; mid < end; ++mid) {
      AABB leftAABB = (*begin)->getAABB();
      AABB rightAABB = (*mid)->getAABB();
      for (auto it = begin; it != mid; ++it) leftAABB = leftAABB.encapsulate((*it)->getAABB());
      for (auto it = mid; it != end; ++it) rightAABB = rightAABB.encapsulate((*it)->getAABB());

      double leftArea = leftAABB.surfaceArea();
      double rightArea = rightAABB.surfaceArea();
      double totalArea = totalAABB.surfaceArea();
      int N_left = std::distance(begin, mid);
      int N_right = std::distance(mid, end);

      double cost = C_trav + ((leftArea / totalArea) * N_left + (rightArea / totalArea) * N_right) * C_inter;

      if (cost < bestCost) {
        bestCost = cost;
        bestSplit = mid;
        bestAxis = axis;
      }
    }
  }

  // If no suitable split was found, create a leaf node
  if (bestAxis == -1) {
    return std::make_unique<Node>(*begin, totalAABB, nullptr, nullptr);
  }

  // Sort shapes one final time along the best axis
  std::sort(begin, end, [bestAxis](const auto& a, const auto& b) {
    return a->getAABB().getCentroid()[bestAxis] < b->getAABB().getCentroid()[bestAxis];
  });

  // Recursively build left and right subtrees
  auto leftTree = buildBspTree(begin, bestSplit);
  auto rightTree = buildBspTree(bestSplit, end);

  // Create a new node with the best split
  return std::make_unique<Node>(nullptr, totalAABB, std::move(leftTree), std::move(rightTree));
}
