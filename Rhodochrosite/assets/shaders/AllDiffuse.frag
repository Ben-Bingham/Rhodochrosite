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
uniform mat4 projectionMatrix;

uniform Sphere spheres[32];
uniform int numberOfSpheres;

uniform DirectionalLight dirLights[8];
uniform int numberOfLights;

uniform float aspectRatio;

uniform float time;

#define FLT_MAX 3.402823466e+38

// Functions
vec3 at(Ray ray, float t) {
	return ray.origin + ray.direction * t;
}

float PHI = 1.61803398874989484820459;  // Golden Ratio   

float randomFloat(float seed = 3.14) {
	vec2 coord = vec2(textureCordinates.x * 1000, textureCordinates.y * 1000 * aspectRatio);
    return fract(tan(distance(coord * PHI, coord) * seed) * coord.x); // 3.14 is seed
}

float randomRange(float minVal, float maxVal, float seed = 3.14) { // [min, max[
	return minVal + (maxVal - minVal) * randomFloat(seed);
}

vec3 randomVec3(float seed = 3.14) {
	return vec3(randomFloat(seed), randomFloat(seed), randomFloat(seed));
}

vec3 randomVec3InRange(float minVal, float maxVal, float seed = 3.14) {
	return vec3(randomRange(minVal, maxVal, seed), randomRange(minVal, maxVal, seed), randomRange(minVal, maxVal, seed));
}

float lengthSquared(vec3 vector) {
	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

vec3 randomVec3InUnitSphere(float seed = 3.14) {
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
		highp float a = dot(ray.direction, ray.direction);
		highp float b = 2.0 * dot(origin, ray.direction);
		highp float c = dot(origin, origin) - (spheres[i].radius * spheres[i].radius);

		highp float discriminant = b * b - 4.0f * a * c;
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

vec4 backgroundColour = vec4(0.6, 0.7, 0.9, 1.0);
int maxNumberOfBounces = 100;

void main() {
	vec2 texCords;
	texCords.x = textureCordinates.x * 2.0 - 1.0;
	texCords.y = (textureCordinates.y * 2.0 - 1.0) * aspectRatio;

	mat4 inverseView = inverse(viewMatrix);
	vec4 target = /*inverse(projectionMatrix) **/ vec4(texCords.x, texCords.y, -1.0, 1.0);
	vec3 rayDirection = vec3(inverseView * vec4(normalize(vec3(target) / target.w), 0));

	Ray ray = Ray( cameraPosition, rayDirection );

	highp vec3 hitPosition;
	vec3 normal;
	Hit hit;
	vec4 colour = vec4(0);
	highp vec3 direction;
	float multiplier = 1.0;
	//int numberOfBounces;

	for (int i = 0; i < maxNumberOfBounces; i++) {
		hit = hitSphere(ray);

		if (hit.hitSomething == false) { // Miss
			colour += backgroundColour * multiplier;
			break;
		}

		// Hit
		vec3 hitLocation = ray.origin + ray.direction * hit.distanceToHit;

		normal = hitLocation - hit.hitSphere.origin;
		normal = normalize(normal);

		float lightIntensity = 0.0;
		for (int i = 0; i < numberOfLights; i++) {
			lightIntensity += max(dot(normal, -dirLights[i].direction), 0.0);
		}

		colour += hit.hitSphere.colour * multiplier * lightIntensity;
		multiplier *= 0.5;


		ray = Ray(hitLocation, reflect(ray.direction, normal + randomVec3InUnitSphere(fract(time))));

		//hit = hitSphere(ray);
		//
		////float lightIntensity = 0.0;
		////for (int i = 0; i < numberOfLights; i++) {
		////	lightIntensity += max(dot(normal, -dirLights[i].direction), 0.0);
		////}
		////
		////lightIntensity = clamp(lightIntensity, 0.0, 1.0);
		//
		//if (hit.hitSomething == false) {
		//	colour += backgroundColour * multiplier/* * lightIntensity*/;
		//	break;
		//}
		//
		//// More lighting 
		//
		//
		//
		////colour = hit.hitSphere.colour * multiplier/* * lightIntensity*/;
		//
		////numberOfBounces++;
		//multiplier *= 0.5;
		//
		//vec3 origin = ray.origin - hit.hitSphere.origin;
		//hitPosition = origin + ray.direction * hit.distanceToHit;
		//
		////hitPosition = at(ray, hit.distanceToHit);
		//
		//normal = normalize(hitPosition);
		//
		//origin = hitPosition + normal * 0.0001;
		//
		////direction = hitPosition + randomVec3InUnitSphere(fract(time));
		//direction = reflect(ray.direction, normal + randomVec3InUnitSphere(fract(time)));
		//
		//ray = Ray(origin, direction);

		//if (i == maxNumberOfBounces - 1) {
		//	FragColor = vec4(0, 0, 0, 1);
		//	return;
		//}


		//hit = hitSphere(ray);
		//
		//if (hit.hitSomething == false) {
		//	colour += backgroundColour * multiplier;
		//	break;
		//}
		//
		////colour += hit.hitSphere.colour * multiplier;
		//
		//hitPosition = at(ray, hit.distanceToHit);
		//
		//normal = normalize(hitPosition - hit.hitSphere.origin);
		//
		//ray = Ray(hitPosition, reflect(normalize(rayDirection), normal + randomVec3InUnitSphere(fract(time))));

		//if (hit.hitSphere.material != 0) {
		//	ray = Ray(hitPosition, reflect(normalize(rayDirection), normal + randomVec3InUnitSphere(fract(time))));
		//}
		//else {
		//	ray = Ray(hitPosition, reflect(normalize(rayDirection), normal));
		//}


		
	}
	//colour /= numberOfBounces;
	FragColor = vec4(colour.xyz, 1);

	//FragColor = vec4(0, 0, 1, 1);
	//return;
	
	//FragColor = vec4(colour.xyz, 1.0);


	//if (hit.hitSomething == false) {
	//	FragColor = vec4(0, 0, 0, 1);
	//	return;
	//}



	//hitPosition = at(ray, hit.distanceToHit);
	//
	//// Lighting Calculations
	//normal = normalize(hitPosition - hit.hitSphere.origin);
	//
	//float lightIntensity = 0.0;
	//for (int i = 0; i < numberOfLights; i++) {
	//	lightIntensity += max(dot(normal, -dirLights[i].direction), 0.0);
	//}
	//
	//lightIntensity = clamp(lightIntensity, 0.0, 1.0);
	//
	//vec4 sphereColour = hit.hitSphere.colour * lightIntensity;
	//FragColor = vec4( sphereColour.x, sphereColour.y, sphereColour.z, 1.0 );
}