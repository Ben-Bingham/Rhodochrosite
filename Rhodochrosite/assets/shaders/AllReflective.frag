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
uniform mat4 viewMatrix;

uniform Sphere spheres[32];
uniform int numberOfSpheres;

uniform DirectionalLight dirLights[8];
uniform int numberOfLights;

uniform float aspectRatio;

#define FLT_MAX 3.402823466e+38

// Functions
vec3 at(Ray ray, float t) {
	return ray.origin + ray.direction * t;
}

float PHI = 1.61803398874989484820459;  // Golden Ratio   

float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

Hit hitSphere(Ray ray) {
	Hit sphereHit;
	sphereHit.distanceToHit = FLT_MAX;
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

		if (hitDistance < sphereHit.distanceToHit) {
			sphereHit.distanceToHit = hitDistance;
			sphereHit.hitSphere = spheres[i];
			sphereHit.hitSomething = true;
		}
	}
	return sphereHit;
}


void main() {
	vec2 texCords;
	texCords.x = textureCordinates.x * 2.0 - 1.0;
	texCords.y = (textureCordinates.y * 2.0 - 1.0) * aspectRatio;

	mat4 inverseView = inverse(viewMatrix);
	vec4 target = vec4(texCords.x, texCords.y, -1.0, 1.0);
	vec3 rayDirection = vec3(inverseView * vec4(normalize(vec3(target) / target.w), 0));

	Ray ray = Ray( cameraPosition, rayDirection );

	vec3 hitPosition;
	vec3 normal;
	Hit hit;

	for (int i = 0; i < 1; i++) {
		hit = hitSphere(ray);

		hitPosition = at(ray, hit.distanceToHit);

		normal = normalize(hitPosition - hit.hitSphere.origin);

		if (hit.hitSphere.material != 0) {
			ray = Ray(hitPosition, reflect(normalize(rayDirection), normal));
		}
		else {
			ray = Ray(hitPosition, reflect(normalize(rayDirection), normal));
		}


		if (hit.hitSomething == false) {
			FragColor = vec4(0, 0, 0, 1);
			return;
		}
	}
	

	//if (hit.hitSomething == false) {
	//	FragColor = vec4(0, 0, 0, 1);
	//	return;
	//}



	hitPosition = at(ray, hit.distanceToHit);

	// Lighting Calculations
	normal = normalize(hitPosition - hit.hitSphere.origin);

	float lightIntensity = 0.0;
	for (int i = 0; i < numberOfLights; i++) {
		lightIntensity += max(dot(normal, -dirLights[i].direction), 0.0);
	}

	lightIntensity = clamp(lightIntensity, 0.0, 1.0);

	vec4 sphereColour = hit.hitSphere.colour * lightIntensity;
	FragColor = vec4( sphereColour.x, sphereColour.y, sphereColour.z, 1.0 );
}