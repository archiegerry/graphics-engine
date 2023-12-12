#include "cone.hpp"

SimpleMeshData make_cone( bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform )
{
	std::vector<Vec3f> pos;
	std::vector<Vec3f> normals;

	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);

	Vec3f capCenter = Vec3f{ 0.f, 0.f, 0.f }; // Center of the cap
	Vec3f capNormalOut = Vec3f{ 1.f, 0.f, 0.f }; // Normal of the cap facing outwards
	Vec3f capNormalIn = Vec3f{ -1.f, 0.f, 0.f }; // Normal of the cap facing inwards

	if (aCapped)
	{
		for (std::size_t i = 0; i < aSubdivs; ++i)
		{
			//calculate angle at current subDivision, based on number of subDivisions
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926535897932384626433832795f;
			// get y and z at current angle
			float y = std::cos(angle);
			float z = std::sin(angle);

			// Cone is aligned along the x-axis
			Vec3f normalSide = normalize(Vec3f{ 1.f, y, z });

			//first triangle of rectangle
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ 0.f, y , z });
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			normals.emplace_back(normalSide);
			
			//front cap
			pos.emplace_back(Vec3f{ -0.f, 0.f, 0.f });  //front cap center vertex
			normals.emplace_back(normalSide); 
			pos.emplace_back(Vec3f{ -0.f, prevY, prevZ });
			normals.emplace_back(normalSide); 
			pos.emplace_back(Vec3f{ -0.f, y, z });
			normals.emplace_back(normalSide);

			// Triangle for the cap (outwards)
			pos.emplace_back(capCenter);
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ 0.f, y, z });
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			normals.emplace_back(normalSide);

			// Triangle for the cap (inwards)
			pos.emplace_back(capCenter);
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ 0.f, y, z });
			normals.emplace_back(normalSide);

			prevY = y;
			prevZ = z;
		}
	}
	else
	{
		for (std::size_t i = 0; i < aSubdivs; ++i)
		{
			//calculate angle at current subDivision, based on number of subDivisions
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926535897932384626433832795f;
			// get y and z at current angle
			float y = std::cos(angle);
			float z = std::sin(angle);

			// Cone is aligned along the x-axis
			Vec3f normalSide = normalize(Vec3f{ 1.f, y, z });

			//first triangle of rectangle
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ 0.f, y , z });
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			normals.emplace_back(normalSide);

			//front cap
			pos.emplace_back(Vec3f{ -0.f, 0.f, 0.f });  //front cap center vertex
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ -0.f, prevY, prevZ });
			normals.emplace_back(normalSide);
			pos.emplace_back(Vec3f{ -0.f, y, z });
			normals.emplace_back(normalSide);

			prevY = y;
			prevZ = z;
		}
	}
		
	//transformation of all positions
	for (size_t i = 0; i < pos.size(); ++i) {
		Vec4f p4{ pos[i].x, pos[i].y, pos[i].z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;
		pos[i] = Vec3f{ t.x, t.y, t.z };

		// Transform normals (ignore translation)
		Vec4f n4{ normals[i].x, normals[i].y, normals[i].z, 1.f };
		Vec4f tn = aPreTransform * n4;
		normals[i] = normalize(Vec3f{ tn.x, tn.y, tn.z });
	}

	SimpleMeshData cone; 
	cone.positions = pos; 
	cone.colors = std::vector<Vec3f>(pos.size(), aColor); 
	cone.normals = normals; 

	return cone; 
}

