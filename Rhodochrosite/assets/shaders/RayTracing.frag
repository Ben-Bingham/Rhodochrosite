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

uniform vec3 cameraPosition;
uniform Sphere sphere;
uniform DirectionalLight dirLight;
uniform float aspectRatio;

void main() {
	vec2 texCords;
	texCords.x = textureCordinates.x * 2.0 - 1.0;
	texCords.y = (textureCordinates.y * 2.0 - 1.0) * aspectRatio;

	Ray ray = Ray( cameraPosition, vec3(texCords.x, texCords.y, -1.0) );

	// Discriminant calculations
	float a = dot(ray.direction, ray.direction);
	float b = 2.0 * dot(ray.origin - sphere.origin, ray.direction);
	float c = dot(ray.origin - sphere.origin, ray.origin - sphere.origin) - (sphere.radius * sphere.radius);

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0) {
		FragColor = vec4(0, 0, 0, 1);
		return;
	}

	// Solving the quadratic formula
	float hitDistance = (-b - sqrt(discriminant)) / 2.0 * a;
	vec3 hitPosition = at(ray, hitDistance);

	// Lighting Calculations
	vec3 normal = normalize(hitPosition - sphere.origin);

	float lightIntensity = max(dot(normal, -dirLight.direction), 0.0);

	vec4 sphereColour = sphere.colour * lightIntensity;
	FragColor = vec4( sphereColour.x, sphereColour.y, sphereColour.z, 1.0 );
}