#version 330 core
out vec4 FragColor;
  
in vec2 textureCordinates;

struct Ray {
	vec3 origin;
	vec3 direction;
};

vec3 at(Ray ray, float t) {
	return ray.origin + ray.direction * t;
}

struct Sphere {
	vec3 origin;
	float radius;
	vec4 colour;
};

struct DirectionalLight {
	vec3 direction;
};

// Uniforms
uniform vec3 cameraPosition;
uniform mat4 viewMatrix;

uniform Sphere spheres[32];
uniform int numberOfSpheres;

uniform DirectionalLight dirLights[8];
uniform int numberOfLights;

uniform float aspectRatio;

#define FLT_MAX 3.402823466e+38

void main() {
	vec2 texCords;
	texCords.x = textureCordinates.x * 2.0 - 1.0;
	texCords.y = (textureCordinates.y * 2.0 - 1.0) * aspectRatio;

	mat4 inverseView = inverse(viewMatrix);
	vec4 target = vec4(texCords.x, texCords.y, -1.0, 1.0);
	vec3 rayDirection = vec3(inverseView * vec4(normalize(vec3(target) / target.w), 0));

	Ray ray = Ray( cameraPosition, rayDirection );

	Sphere hitSphere;
	bool hitSomething = false;
	float closestHit = FLT_MAX;
	for (int i = 0; i < numberOfSpheres; i++) {

		// Discriminant calculations
		float a = dot(ray.direction, ray.direction);
		float b = 2.0 * dot(ray.origin - spheres[i].origin, ray.direction);
		float c = dot(ray.origin - spheres[i].origin, ray.origin - spheres[i].origin) - (spheres[i].radius * spheres[i].radius);

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0) {
			continue;
		}

		// Solving the quadratic formula
		float hitDistance = (-b - sqrt(discriminant)) / 2.0 * a;
		if (hitDistance < 0.0) {
			continue;
		}

		if (hitDistance < closestHit) {
			closestHit = hitDistance;
			hitSphere = spheres[i];
			hitSomething = true;
		}
	}

	if (hitSomething == false) {
		FragColor = vec4(0, 0, 0, 1);
		return;
	}

	vec3 hitPosition = at(ray, closestHit);

	// Lighting Calculations

	vec3 normal = normalize(hitPosition - hitSphere.origin);
	
	float lightIntensity = 0.0;
	for (int i = 0; i < numberOfLights; i++) {
		lightIntensity += max(dot(normal, -dirLights[i].direction), 0.0);
	}

	lightIntensity = clamp(lightIntensity, 0.0, 1.0);

	vec4 sphereColour = hitSphere.colour * lightIntensity;
	FragColor = vec4( sphereColour.x, sphereColour.y, sphereColour.z, 1.0 );
}