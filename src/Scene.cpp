#include "Scene.h"
#include "Utility.h"

#include <utility>
#include <cmath>
#include <random>
#include <iostream>

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
      renderMode(renderMode) {}

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

  for (unsigned int y = 0; y < camera.getHeight(); y++) {
    for (unsigned int x = 0; x < camera.getWidth(); x++) {
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

  for (unsigned int y = 0; y < camera.getHeight(); y++) {
    printProgress(y, camera.getHeight());
    for (unsigned int x = 0; x < camera.getWidth(); x++) {
      image.setColor(x, y, sample(x, y, 8, colourSampler));
    }
  }
  printProgress(camera.getHeight(), camera.getHeight());
  return image;
}

Image Scene::renderBlinnPhong() {
  Image image(camera.getWidth(), camera.getHeight());

  auto colourSampler = [this](const Scene &s, const Ray &r) { return this->sampleBlinnPhong(r); };

  for (unsigned int y = 0; y < camera.getHeight(); y++) {
    printProgress(y, camera.getHeight());
    for (unsigned int x = 0; x < camera.getWidth(); x++) {
      image.setColor(x, y, sample(x, y, 8, colourSampler));
    }
  }
  printProgress(camera.getHeight(), camera.getHeight());
  return image;
}

Image Scene::renderPathtracer() {
  Image image(camera.getWidth(), camera.getHeight());

  auto colourSampler = [this](const Scene &s, const Ray &r) { return this->samplePathtracer(r); };

  for (unsigned int y = 0; y < camera.getHeight(); y++) {
    printProgress(y, camera.getHeight());
    for (unsigned int x = 0; x < camera.getWidth(); x++) {
      image.setColor(x, y, sample(x, y, 10, colourSampler));
    }
  }
  printProgress(camera.getHeight(), camera.getHeight());
  return image;
}

Colour Scene::sample(unsigned int x,
                     unsigned int y,
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
    Vector3D reflectDirection = ray.getDirection().reflect(normal);
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

  bool isExitRay = ray.getDirection().dot(normal) > 0;

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
    Vector3D reflectDir = ray.getDirection().reflect(normal).normalize();
    Ray reflectRay(hitPoint, reflectDir); // Avoid self-intersection
    Colour reflectSample = sampleBlinnPhong(reflectRay, depth + 1);
    colour = Colour(colour * (1 - material.getReflectivity())
                        + reflectSample * material.getReflectivity());
  }
  if (material.getIsRefractive()) {
    if (isExitRay) {
      Vector3D refractDir =
          ray.getDirection().refract(-normal, 1 / material.getRefractiveIndex()).normalize();
      Ray refractRay(hitPoint, refractDir);
      auto refractIntersection = checkIntersection(refractRay);
      Colour refractSample = sampleBlinnPhong(refractRay, depth + 1);
      colour = refractSample;

    } else {
      Vector3D refractDir =
          ray.getDirection().refract(normal, material.getRefractiveIndex()).normalize();
      Ray refractRay(hitPoint, refractDir);
      auto refractIntersection = checkIntersection(refractRay);
      Colour refractSample = sampleBlinnPhong(refractRay, depth + 1);
      colour = refractSample;
    }
  }

  // Clamp colour values to ensure they are within the valid range
  colour = colour.clamp();

  return colour;
}

bool Scene::isInShadow(const Vector3D &point, const LightSource &light) const {
  Vector3D lightDir = (light.getPosition() - point).normalize();
  Ray shadowRay(point, lightDir);
  Interval interval = Interval(EPSILON, (light.getPosition() - point).magnitude());
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(shadowRay, interval);
  return intersection.has_value();
}

Colour Scene::samplePathtracer(const Ray &ray, int depth) {
  // Base case: if we've reached the maximum number of bounces, return black
  if (depth >= nBounces) return backgroundColour;

  // Check if the ray intersects with any objects in the scene
  std::optional<std::pair<std::shared_ptr<Shape>, double>> intersection = checkIntersection(ray);

  // If the ray doesn't intersect with anything, return the background backgroundColour
  if (!intersection.has_value()) return backgroundColour;

  auto [hitShape, hitDistance] = intersection.value();

  Vector3D hitPoint = ray.at(hitDistance);
  Vector3D normal = hitShape->getSurfaceNormal(ray.at(intersection.value().second));
  Material material = hitShape->getMaterial();

  // Specular illumination
  Colour specularIllumination(0, 0, 0);
  double specularExponent = material.getSpecularExponent();
  Colour specularColour = material.getSpecularColour();
  for (const auto &lightSource : lightSources) {
    auto [toLight, distanceToLight] = lightSource->getDirectionAndDistance(hitPoint);
    Vector3D lightDirection = toLight.normalize();

    if (!isInShadow(hitPoint, *lightSource)) {
      // Calculate the reflection of the light direction around the normal
      Vector3D reflectionDirection = lightDirection.reflect(normal).normalize();

      // Calculate the specular term
      double specAngle = std::max(reflectionDirection.dot(ray.getDirection().normalize()), 0.0);
      double specularTerm = std::pow(specAngle, specularExponent);

      // Attenuate the light based on the distance and add it to the direct illumination
      Colour lightIntensity = Colour(lightSource->getIntensity() / (4 * M_PI * distanceToLight * distanceToLight));
      specularIllumination += specularColour * lightIntensity * specularTerm;
    }
  }

  // Diffuse illumination

  // Get the diffuse factor and backgroundColour of the object
  double diffuseFactor = intersection.value().first->getMaterial().getKd();
  Colour diffuseColour = intersection.value().first->getMaterial().getDiffuseColour();

  // Direct illumination
  Colour directIllumination(0, 0, 0);
  for (const auto &lightSource : lightSources) {
    Vector3D toLight = lightSource->getPosition() - hitPoint;
    double distanceToLight = toLight.magnitude();
    Vector3D lightDirection = toLight.normalize();

    if (!isInShadow(hitPoint, *lightSource)) {
      // Compute the illumination contribution
      double nDotL = std::max(normal.dot(toLight), 0.0);
      Colour lightIntensity = Colour(lightSource->getIntensity() / (4 * M_PI * distanceToLight * distanceToLight));
      directIllumination += material.getDiffuseColour() * lightIntensity * nDotL;
    }
  }

  // Indirect illumination
  int nSamples = 10;
  Colour bounceColour(0, 0, 0);
  for (int i = 0; i < nSamples; i++) {
    // Get a random direction in the hemisphere of the normal
    Vector3D bounceDirection = normal + normal.randomInHemisphere();

    // Create a new ray starting at the intersection point and going in the bounce direction
    Ray bounceRay = Ray(ray.at(intersection.value().second), bounceDirection);

    // Calculate the backgroundColour of the bounce ray
    bounceColour += Colour(sampleDiffuse(bounceRay, depth + 1) / nSamples);
  }

  return Colour(specularIllumination + directIllumination + bounceColour * diffuseFactor * diffuseColour);
}

std::optional<std::pair<std::shared_ptr<Shape>, double>> Scene::checkIntersection(const Ray &ray,
                                                                                  Interval interval) const {
  std::shared_ptr<Shape> closestShape = nullptr;
  double closestT = 0;
  for (const auto &shape : shapes) {
    std::optional<double> t = shape->checkIntersection(ray, interval);
    if (t.has_value() && (t.value() < closestT || closestShape == nullptr)) {
      closestShape = shape;
      closestT = t.value();
    }
  }
  if (closestShape == nullptr) {
    return std::nullopt;
  } else {
    return std::make_pair(closestShape, closestT);
  }
}

// Print the progress as a percentage as a whole number
void Scene::printProgress(unsigned int current, unsigned int total) {
  int progress = (int) (100.0 * current / total);
  std::cout << "\rRendering: " << progress << "% complete" << std::flush;
}
