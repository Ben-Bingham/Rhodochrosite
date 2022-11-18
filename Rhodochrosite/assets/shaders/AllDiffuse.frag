#version 330 core
out vec4 FragColor;

in vec2 textureCordinates;

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Sphere {
	vec3 origin;
	float radius;
	vec4 colour;
	int material; // DIFFUSE = 0, REFLECTION = 1, REFRACTION = 2
};

struct DirectionalLight {
	vec3 direction;
};

struct Hit {
	bool hitSomething;
	Sphere hitSphere;
	float distanceToHit;
};

// Uniforms
uniform vec3 cameraPosition;
uniform vec3 cameraDirection;

uniform Sphere spheres[32];
uniform int numberOfSpheres;

uniform DirectionalLight dirLights[8];
uniform int numberOfLights;

uniform float aspectRatio;
uniform int pixelWidth;
uniform int pixelHeight;

uniform float time;

#define FLT_MAX 3.402823466e+38

// Functions
vec3 at(Ray ray, float t) {
	return ray.origin + ray.direction * t;
}

float PHI = 1.61803398874989484820459;  // Golden Ratio   

float randomFloat(float seed) {
	vec2 coord = vec2(textureCordinates.x * 1000, textureCordinates.y * 1000 * aspectRatio);
    return fract(tan(distance(coord * PHI, coord) * tan(seed)) * coord.x);
}

float randomRange(float minVal, float maxVal, float seed) { // [min, max[
	return minVal + (maxVal - minVal) * randomFloat(seed);
}

vec3 randomVec3(float seed) {
	return vec3(randomFloat(seed), randomFloat(seed), randomFloat(seed));
}

vec3 randomVec3InRange(float minVal, float maxVal, float seed) {
	return vec3(randomRange(minVal, maxVal, seed), randomRange(minVal, maxVal, seed), randomRange(minVal, maxVal, seed));
}

float lengthSquared(vec3 vector) {
	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

vec3 randomVec3InUnitSphere(float seed) {
	int i = 0;
	while (true) {
		i++;
        vec3 vec = randomVec3InRange(-1,1, seed * i);
        if (lengthSquared(vec) >= 1) continue;
        return vec;
	}
}

Hit hitSphere(Ray ray) {
	Hit sphereHit;
	sphereHit.hitSomething = false;
	sphereHit.distanceToHit = FLT_MAX;
	for (int i = 0; i < numberOfSpheres; i++) {

		vec3 origin = ray.origin - spheres[i].origin;

		// Discriminant calculations
		float a = dot(ray.direction, ray.direction);
		float b = 2.0 * dot(origin, ray.direction);
		float c = dot(origin, origin) - (spheres[i].radius * spheres[i].radius);

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0) {
			continue;
		}

		// Solving the quadratic formula
		float hitDistance = (-b - sqrt(discriminant)) / 2.0 * a;
		if (hitDistance < 0.001) {
			continue;
		}

		if (hitDistance < sphereHit.distanceToHit) {
			sphereHit.distanceToHit = hitDistance;
			sphereHit.hitSphere = spheres[i];
			sphereHit.hitSomething = true;
		}
	}
	return sphereHit;
}

Ray scatterRayDiffuse(Ray incidentRay, Hit hit, vec3 hitLocation, vec3 normal, float randSeed) {
	return Ray(hitLocation, reflect(incidentRay.direction, normal + randomVec3InUnitSphere(randSeed + 3.0)));
}

vec3 backgroundColour = vec3(0.6, 0.7, 0.9);
int maxNumberOfBounces = 50;
int numberOfSamples = 4;
float distanceToImagePlane = 1.0;

void main() {
	highp float randSeed = (time / time - time - time * time + time) / time;

	vec3 colour = vec3(0, 0, 0);
	for (int i = 0; i < numberOfSamples; i++) {
		float randX = randomRange(0, 1 / pixelWidth, randSeed + i);
		float randY = randomRange(0, 1 / pixelHeight, randSeed + i);

		vec2 texCords;
		texCords.x = (textureCordinates.x + randX) * 2.0 - 1.0;
		texCords.y = ((textureCordinates.y + randY) * 2.0 - 1.0) * aspectRatio;

		vec3 cameraRight = cross(cameraDirection, vec3(0, -1, 0));
		vec3 cameraUp = cross(cameraDirection, cameraRight);

		vec3 imagePoint = ((-texCords.x) * cameraRight) + ((texCords.y) * cameraUp) + (cameraDirection * distanceToImagePlane) + cameraPosition;
		vec3 rayDirection = imagePoint - cameraPosition;

		Ray ray = Ray(cameraPosition, normalize(rayDirection));

		vec3 tempColour = vec3(0);
		float multiplier = 1.0;
		for (int i = 0; i < maxNumberOfBounces; i++) {
			Hit hit = hitSphere(ray);

			if (hit.hitSomething == false) { // Miss
				tempColour += backgroundColour * multiplier;
				break;
			}

			vec3 hitLocation = ray.origin + ray.direction * hit.distanceToHit;
			vec3 normal = normalize(hitLocation - hit.hitSphere.origin);

			float lightIntensity = 0.0;
			for (int i = 0; i < numberOfLights; i++) {
				lightIntensity += max(dot(normal, -dirLights[i].direction), 0.0);
			}
			
			lightIntensity = clamp(lightIntensity, 0.0, 1.0);

			tempColour += hit.hitSphere.colour.xyz * multiplier * lightIntensity;
			multiplier *= 0.5;

			ray = scatterRayDiffuse(ray, hit, hitLocation, normal, cos(randSeed + i + 4234.634553));
		}
		colour += tempColour;
	}

	colour /= numberOfSamples;
	FragColor = vec4(colour, 1);
	return;
}