#include "Scene.h"
#include "Utility.h"

#include <utility>
#include <cmath>
#include <random>
#include <iostream>
#include <omp.h>

Scene::Scene(Colour colour,
             const Camera &camera,
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

Scene::Scene(JsonObject json) : nBounces(json["nbounces"].asInt()), camera(json["camera"].asObject()) {
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
  Image image(camera.getWidth(), camera.getHeight());

  for (int y = 0; y < camera.getHeight(); y++) {
    for (int x = 0; x < camera.getWidth(); x++) {
      Ray ray = camera.getRay(x, y);
      std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

      Colour pixelColour;
      if (intersection.has_value()) {
        pixelColour = intersection.value().first->getMaterial().getDiffuseColour();
      } else {
        pixelColour = backgroundColour;
      }

      image.setColor(x, y, pixelColour);
    }
  }
  return image;
}

Image Scene::renderShaded() {
  Image image(camera.getWidth(), camera.getHeight());

  auto colourSampler = [this](const Scene &s, const Ray &r) { return this->sampleDiffuseAndSpecular(r); };

  for (int y = 0; y < camera.getHeight(); y++) {
    printProgress(y, camera.getHeight());
    for (int x = 0; x < camera.getWidth(); x++) {
      image.setColor(x, y, sample(x, y, 8, colourSampler));
    }
  }
  printProgress(camera.getHeight(), camera.getHeight());
  return image;
}

Image Scene::renderBlinnPhong() {
  Image image(camera.getWidth(), camera.getHeight());

  auto colourSampler = [this](const Scene &s, const Ray &r) { return this->sampleBlinnPhong(r); };

  int done = 0;
  auto start = std::chrono::high_resolution_clock::now();

  printProgress(0, camera.getHeight());

#pragma omp parallel for num_threads(8) schedule(dynamic) default(none) shared(image, colourSampler, start, done)
  for (int y = 0; y < camera.getHeight(); y++) {
    for (int x = 0; x < camera.getWidth(); x++) {
      image.setColor(x, y, sample(x, y, 8, colourSampler));
    }
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    printProgress(++done, camera.getHeight(), elapsed);
  }
  return image;
}

Image Scene::renderPathtracer() {
  Image image(camera.getWidth(), camera.getHeight());
  Image varianceImage(camera.getWidth(), camera.getHeight());

  auto colourSampler = [this](const Scene &s, const Ray &r) { return this->samplePathtracer(r).colour; };

  int baselineSamples = 10;
  int additionalSamplesMax = 90;

  int done = 0;
  auto start = std::chrono::high_resolution_clock::now();

  printProgress(0, camera.getHeight());

#pragma omp parallel for num_threads(8) schedule(dynamic) default(none) shared(image, colourSampler, start, done, baselineSamples, varianceImage)
  for (int y = 0; y < camera.getHeight(); y++) {
    for (int x = 0; x < camera.getWidth(); x++) {
      Colour pixelColour;
      std::vector<Colour> samples;
      double scale = 1.0 / baselineSamples;
      for (int i = 0; i < baselineSamples; i++) {
        double xOffset = random_double();
        double yOffset = random_double();
        Ray ray = camera.getRay(x, y, xOffset, yOffset);
        auto [colour, distance] = samplePathtracer(ray);
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
    printProgress(++done, camera.getHeight(), elapsed);
  }

#pragma omp parallel for num_threads(8) schedule(dynamic) default(none) shared(image, colourSampler, start, done, baselineSamples, varianceImage, additionalSamplesMax)
  for (int y = 0; y < camera.getHeight(); y++) {
    for (int x = 0; x < camera.getWidth(); x++) {
      Colour pixelColour = image.getColor(x, y);
      Colour pixelVariance = varianceImage.getColor(x, y);
      double variance = pixelVariance.red() + pixelVariance.green() + pixelVariance.blue();
      double additionalSamples = std::max(0.0, std::min(1.0, std::sqrt(variance)));
      int nSamples = additionalSamples * additionalSamplesMax;
      double scale = 1.0 / (nSamples + baselineSamples);
      pixelColour = Colour(pixelColour * ((double)baselineSamples / (baselineSamples + nSamples)));
      for (int i = 0; i < nSamples; i++) {
          double xOffset = random_double();
          double yOffset = random_double();
          Ray ray = camera.getRay(x, y, xOffset, yOffset);
          auto [colour, distance] = samplePathtracer(ray);
          pixelColour += colour * scale;
      }
      image.setColor(x, y, pixelColour);
    }
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    printProgress(++done, camera.getHeight(), elapsed);
  }

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
    Ray ray = camera.getRay(x, y, xOffset, yOffset);
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

Colour Scene::sampleDiffuse(const Ray &ray, int depth) {
  // Base case: if we've reached the maximum number of bounces, return black
  if (depth >= nBounces) return backgroundColour;

  // Check if the ray intersects with any objects in the scene
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

  // If the ray doesn't intersect with anything, return the background backgroundColour
  if (!intersection.has_value()) return backgroundColour;

  // Get the normal of the surface at the intersection point
  Vector3D normal = intersection.value().first->getSurfaceNormal(ray.at(intersection.value().second));

  // Get a random direction in the hemisphere of the normal
  Vector3D bounceDirection = normal + normal.randomInHemisphere();

  // Create a new ray starting at the intersection point and going in the bounce direction
  Ray bounceRay = Ray(ray.at(intersection.value().second), bounceDirection);

  // Get the diffuse factor and backgroundColour of the object
  double diffuseFactor = intersection.value().first->getMaterial().getKd();
  Colour diffuseColour = intersection.value().first->getMaterial().getDiffuseColour();

  // Calculate the backgroundColour of the bounce ray
  Colour bounceColour = sampleDiffuse(bounceRay, depth + 1);

  return Colour(bounceColour * diffuseFactor * diffuseColour);
}

Colour Scene::sampleDiffuseAndSpecular(const Ray &ray, int depth) {
  if (depth >= nBounces) return backgroundColour;

  auto intersection = checkIntersection(ray);
  if (!intersection) return backgroundColour;

  auto [hitShape, hitDistance] = *intersection;
  Vector3D hitPoint = ray.at(hitDistance);
  Vector3D normal = hitShape->getSurfaceNormal(hitPoint).normalize();
  Material material = hitShape->getMaterial();

  Colour ambientColour = Colour(); // Define your scene's ambient light colour if any
  Colour colour = ambientColour; // Initialize with ambient light

  // Sample diffuse component
  Vector3D bounceDirection = normal.randomInHemisphere();
  Ray bounceRay(hitPoint + bounceDirection, bounceDirection);
  Colour bounceColour = sampleDiffuseAndSpecular(bounceRay, depth + 1);
  Colour diffuseColour = material.getDiffuseColour();
  colour += bounceColour * material.getKd() * diffuseColour;

  // Compute the specular component for each light source
  Colour specColour = Colour();
  for (const auto &lightSource : lightSources) {
    Vector3D lightDir = (lightSource->getPosition() - hitPoint).normalize();
    Vector3D reflectionDir = (-lightDir).reflect(normal).normalize();
    Vector3D viewDir = (camera.getPosition() - hitPoint).normalize();

    if (!isInShadow(hitPoint, *lightSource)) {
      double specIntensity = std::pow(std::max(viewDir.dot(reflectionDir), 0.0), material.getSpecularExponent());
      specColour += lightSource->getIntensity() * specIntensity * material.getSpecularColour();
    }
  }
  colour += specColour * material.getKs();

  // Reflectivity
  if (material.getIsReflective()) {
    Vector3D reflectDirection = ray.direction.reflect(normal);
    Ray reflectRay(hitPoint, reflectDirection);
    Colour reflectColour = sampleDiffuseAndSpecular(reflectRay, depth + 1);
    colour += reflectColour * material.getReflectivity();
  }

  return colour;
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
  Material material = hitShape->getMaterial();

  bool isExitRay = ray.direction.dot(normal) > 0;

  // Initialize the colour with ambient light if defined, otherwise black
  Colour ambientIntensity = Colour(0.3, 0.3, 0.3);
  Colour ambientColour = Colour(ambientIntensity * material.getDiffuseColour());

  Colour diffuseSpecularSum = Colour();

  // Diffuse and specular calculations for each light source
  for (const auto &lightSource : lightSources) {
    Vector3D lightDir = (lightSource->getPosition() - hitPoint).normalize();
    Vector3D viewDir = (camera.getPosition() - hitPoint).normalize();
    Vector3D halfDir = (lightDir + viewDir).normalize();
    double lightDistance = (lightSource->getPosition() - hitPoint).magnitude();

    // Check for shadow
    if (!isInShadow(hitPoint, *lightSource)) {
      // Diffuse term
      double ln = lightDir.dot(normal);
      Colour id = Colour(lightSource->getIntensity() * material.getDiffuseColour());
      Colour diffuseColour = Colour(id * ln * material.getKd());
      diffuseSpecularSum += diffuseColour * material.getKd();

      // Blinn-Phong specular term
      double hn = halfDir.dot(normal);
      Colour is = Colour(lightSource->getIntensity() * material.getSpecularColour());
      Colour specularColour = Colour(is * std::pow(hn, material.getSpecularExponent()) * material.getKs());
      diffuseSpecularSum += specularColour * material.getKs() / (lightDistance * lightDistance);
    }
  }

  // Reflectivity and refraction
  Colour colour = Colour(ambientColour + diffuseSpecularSum);
  if (material.getIsReflective()) {
    Vector3D reflectDir = ray.direction.reflect(normal).normalize();
    Ray reflectRay(hitPoint, reflectDir); // Avoid self-intersection
    Colour reflectSample = sampleBlinnPhong(reflectRay, depth + 1);
    colour = Colour(colour * (1 - material.getReflectivity())
                        + reflectSample * material.getReflectivity());
  }
  if (material.getIsRefractive()) {
    Vector3D refractDir;
    if (isExitRay) {
      refractDir = ray.direction.refract(normal, 1 / material.getRefractiveIndex()).normalize();
    } else {
      refractDir = ray.direction.refract(normal, material.getRefractiveIndex()).normalize();
    }
    Ray refractRay(hitPoint, refractDir);
    Colour refractSample = sampleBlinnPhong(refractRay, depth + 1);
    colour = refractSample;
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

Colour Scene::isInShadowPathtracer(const Ray &shadowRay, double maxDistance, const LightSource &light) const {
  Interval interval = Interval(0.0001, maxDistance);
  std::unique_ptr<Ray> currentRay = std::make_unique<Ray>(shadowRay);
  Colour result = Colour(light.getIntensity());

  while (true) {
    auto intersection = checkIntersection(*currentRay, interval);
    if (!intersection.has_value()) {
      return result;
    }

    auto [hitShape, hitDistance] = intersection.value();
    Vector3D hitPoint = currentRay->at(hitDistance);
    Material material = hitShape->getMaterial();

    if (material.getIsRefractive()) {
      // Calculate the refractive ray
      double refractiveIndex = material.getRefractiveIndex();
      Vector3D normal = hitShape->getSurfaceNormal(hitPoint);
      bool isExitRay = currentRay->direction.dot(normal) > 0;
      Vector3D refractDirection;
      if (isExitRay) {
        refractDirection = currentRay->direction.refract(-normal, 1 / refractiveIndex).normalize();
      } else {
        refractDirection = currentRay->direction.refract(normal, refractiveIndex).normalize();
      }

      currentRay = std::make_unique<Ray>(Ray(hitPoint + refractDirection, refractDirection));
//      result *= material.getTransparency(); // Accumulate transparency of refractive materials
      result = Colour(result * 0.9);
      interval = Interval(0.0001, maxDistance - hitDistance);
    } else {
      return Colour(0, 0, 0);
    }
  }
}

SampleRecord Scene::samplePathtracer(const Ray &ray, int depth) {
  // Base case: if we've reached the maximum number of bounces, return black
  if (depth >= nBounces) return {backgroundColour, INFINITY};

  // Check if the ray intersects with any objects in the scene
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

  // If the ray doesn't intersect with anything, return the background backgroundColour
  if (!intersection.has_value()) return {backgroundColour, INFINITY};

  auto [hitShape, hitDistance] = intersection.value();

  Vector3D hitPoint = ray.at(hitDistance);
  Vector3D normal = hitShape->getSurfaceNormal(ray.at(intersection.value().second));
  Material material = hitShape->getMaterial();

  // Reflectivity
  Colour reflectionColour(0, 0, 0);
  double reflectivity = material.getReflectivity();
  if (!material.getIsReflective()) {
    reflectivity = 0;
  }

  if (reflectivity > 0) {
    Vector3D reflectDirection = ray.direction.reflect(normal, material.getRoughness());
    Ray reflectRay(hitPoint, reflectDirection);
    auto [newReflectionColour, reflectionDistance] = samplePathtracer(reflectRay, depth + 1);
    reflectionColour = newReflectionColour;

    if (reflectivity == 1) {
      return {reflectionColour, hitDistance};
    }
  }

  // Refraction
  Colour refractionColour(0, 0, 0);
  if (material.getIsRefractive()) {
    double refractiveIndex = material.getRefractiveIndex();
    Vector3D refractDirection;
    bool isExitRay = ray.direction.dot(normal) > 0;
    if (isExitRay) {
      refractDirection = ray.direction.refract(-normal, 1 / refractiveIndex);
    } else {
      refractDirection = ray.direction.refract(normal, refractiveIndex);
    }
    refractDirection = (refractDirection + normal.randomInHemisphere() * material.getRoughness()).normalize();
    Ray refractRay(hitPoint, refractDirection);
    auto [newRefractionColour, refractionDistance] = samplePathtracer(refractRay, depth + 1);
    refractionColour = newRefractionColour;
  }

  // Diffuse illumination

  // Get the diffuse factor and backgroundColour of the object
  double diffuseFactor = intersection.value().first->getMaterial().getKd();
  Colour diffuseColour = intersection.value().first->getMaterial().getDiffuseColour();

  // Direct illumination
  Colour directIllumination(0, 0, 0);
  for (const auto &lightSource : lightSources) {
    for (int i = 0; i < lightSource->samplingFactor; i++) {
      auto [toLight, distanceToLight] = lightSource->getDirectionAndDistance(hitPoint);
      Vector3D lightDirection = toLight.normalize();
      Vector3D halfDirection = (lightDirection + ray.direction).normalize();

      Colour lightIntensity = isInShadowPathtracer(Ray(hitPoint, toLight), distanceToLight, *lightSource);

      if (lightIntensity.max() > 0) {
        // Compute the illumination contribution
        // Diffuse Contribution
        double lambertian = std::min(std::max(normal.dot(toLight), 0.0), 1.0);
        Colour diffuseContribution = Colour(diffuseColour * lambertian * diffuseFactor * lightIntensity);

        // Specular Contribution
        double specularCoefficient = pow(std::max(normal.dot(halfDirection), 0.0), material.getSpecularExponent());
        Colour specularContribution =
            Colour(material.getSpecularColour() * specularCoefficient * lightIntensity);

        // Combine contributions
        directIllumination += (diffuseContribution + specularContribution)
            / (lightSource->samplingFactor * distanceToLight * distanceToLight);
      }
    }
  }

  // Russian Roulette termination
  if (depth > 3) {
    double terminationProbability = std::max(0.05, 1 - reflectionColour.max());
    double randomValue = random_double();

    if (randomValue < terminationProbability) {
      return {Colour(directIllumination), hitDistance};
    } else {
      directIllumination = Colour(directIllumination / (1 - terminationProbability));
    }
  }

  // Indirect illumination
  int nSamples = std::max(1, 10 - depth * 2);
  Colour bounceColour(0, 0, 0);
  for (int i = 0; i < nSamples; i++) {
    // Get a random direction in the hemisphere of the normal
    Vector3D bounceDirection = normal + normal.randomInHemisphere();  // * material.getKd();

    // Create a new ray starting at the intersection point and going in the bounce direction
    Ray bounceRay = Ray(ray.at(hitDistance), bounceDirection.normalize());

    auto [newBounceColour, bounceDistance] = samplePathtracer(bounceRay, depth + 1);

    // Calculate the colour of the bounce ray
    bounceColour += Colour(newBounceColour / nSamples);  // (nSamples * bounceDistance * bounceDistance));
  }

  if (!material.getIsRefractive()) {
    Colour totalIllumination = Colour(directIllumination + bounceColour * diffuseFactor * diffuseColour);
    return {Colour(
        totalIllumination * (1 - reflectivity)
            + reflectionColour * reflectivity
    ), hitDistance};
  } else {
    Colour totalIllumination = Colour(directIllumination + bounceColour * diffuseFactor * diffuseColour);
    return {Colour(
        totalIllumination * reflectivity
            + reflectionColour * reflectivity
            + refractionColour * (1 - reflectivity)
    ), hitDistance};
  }
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
