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

//float randomFloat(float seed) {
//	vec2 coord = vec2(textureCordinates.x * 1000, textureCordinates.y * 1000 * aspectRatio);
//    return fract(tan(distance(coord * PHI, coord) * tan(seed)) * coord.x);
//}

// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}


float randomFloat(float seed) {
	return noise(vec2(gl_FragCoord.x / pixelWidth, gl_FragCoord.y / pixelHeight) * (100.0 + sin(seed) * 32424.423456));
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

vec3 backgroundColour = vec3(0.6, 0.7, 0.9);
int maxNumberOfBounces = 2;
float distanceToImagePlane = 1.0;

Ray scatterRayReflective(Ray incidentRay, Hit hit) {
	vec3 hitLocation = incidentRay.origin + incidentRay.direction * hit.distanceToHit;
	vec3 normal = normalize(hitLocation - hit.hitSphere.origin);
	return Ray(hitLocation, reflect(incidentRay.direction, normal));
}

Ray scatterRayDiffuse(Ray incidentRay, Hit hit, float randSeed) {
	vec3 hitLocation = incidentRay.origin + incidentRay.direction * hit.distanceToHit;
	vec3 normal = normalize(hitLocation - hit.hitSphere.origin);
	return Ray(hitLocation, reflect(incidentRay.direction, normal + randomVec3InUnitSphere(randSeed + 3.0)));
}

void main() {
	highp float randSeed = (time / time - time - time * time + time) / time;

	//vec3 random = randomVec3InRange(0, 1, randSeed + 3.14);
	//if (random.x < -1 || random.y < -1 || random.z < -1) {
	//	FragColor = vec4(1, 0, 0, 1);
	//}
	//else {
	//	FragColor = vec4(0, 1, 0, 1);
	//}
	//
	//FragColor = vec4(randomVec3InUnitSphere(randSeed + 2.0f), 1);
	//
	//return;

	vec2 texCords;
	texCords.x = textureCordinates.x * 2.0 - 1.0;
	texCords.y = (textureCordinates.y * 2.0 - 1.0) * aspectRatio;

	vec3 cameraRight = cross(cameraDirection, vec3(0, -1, 0));
	vec3 cameraUp = cross(cameraDirection, cameraRight);

	vec3 imagePoint = (-texCords.x * cameraRight) + (texCords.y * cameraUp) + (cameraDirection * distanceToImagePlane) + cameraPosition;
	vec3 rayDirection = imagePoint - cameraPosition;

	Ray ray = Ray(cameraPosition, normalize(rayDirection));

	//vec3 hitPosition;
	vec3 colour = vec3(0, 0, 0);
	//float multiplier = 1.0;

	for (int i = 0; i < maxNumberOfBounces; i++) {
		Hit hit = hitSphere(ray);

		if (hit.hitSomething == false) { // Miss
			//colour += backgroundColour * multiplier;
			colour = backgroundColour/* * multiplier*/;
			break;
		}
		//else { 
		// Hit
		colour += hit.hitSphere.colour.xyz/* * multiplier*/;

		//vec3 hitLocation = ray.origin + ray.direction * hit.distanceToHit;
		//vec3 normal = normalize(hitLocation - hit.hitSphere.origin);
		
		//vec3 rayDirection = reflect(ray.direction, normal + randomVec3InUnitSphere(randSeed + 3.0).xyz);

		//float val = hit.distanceToHit;
		//val /= 20;


		//FragColor = vec4(val, val, val, 1);
		//
		//return;

		//multiplier *= 0.5;



		ray = scatterRayDiffuse(ray, hit, randSeed);
		//vec3 hitLocation = ray.origin + ray.direction * hit.distanceToHit;
		//vec3 normal = normalize(hitLocation - hit.hitSphere.origin);
		//
		//ray = Ray(hitLocation, reflect(ray.direction, normal));



			//
			//hit = hitSphere(ray);
			//
			//// Recurse
			//if (hit.hitSomething == false) { // Miss
			//	colour = backgroundColour;
			//	break;
			//}
			//else { // Hit
			//	colour += hit.hitSphere.colour.xyz * multiplier;
			//	break;
			//}
		//}

		//// Hit
		//vec3 hitLocation = ray.origin + ray.direction * hit.distanceToHit;
		//
		//vec3 normal = normalize(hitLocation - hit.hitSphere.origin);
		//
		//float lightIntensity = 0.0;
		//for (int i = 0; i < numberOfLights; i++) {
		//	lightIntensity += max(dot(normal, -dirLights[i].direction), 0.0);
		//}
		//
		//colour += hit.hitSphere.colour.xyz * multiplier * lightIntensity;
		//multiplier *= 0.5;
		//
		//ray = Ray(hitLocation + (normal * 0.001), reflect(ray.direction, normal + normalize(randomVec3InUnitSphere(fract(time)))));

		
	}
	FragColor = vec4(colour, 1);


























	
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