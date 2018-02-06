#pragma once

#include "Resource.h"

struct Vertex
{
	Vec3 pos;
	Vec3 normal;
	Vec2 tex;
};

struct AniVertex
{
	Vec3 pos;
	float weights;
	uint indices;
	Vec3 normal;
	Vec2 tex;
};

struct Mesh final : Resource
{
	enum MESH_FLAGS
	{
		F_TANGENTS = 1 << 0,
		F_ANIMATED = 1 << 1,
		F_STATIC = 1 << 2,
		F_PHYSICS = 1 << 3,
		F_SPLIT = 1 << 4
	};

	struct Header
	{
		char format[4];
		byte version, flags;
		word n_verts, n_tris, n_subs, n_bones, n_anims, n_points, n_groups;
		float radius;
		Box bbox;
		uint points_offset;
		Vec3 cam_pos, cam_target, cam_up;
	};

	struct Submesh
	{
		word first; // pierwszy strójk¹t do narysowania
		word tris; // ile trójk¹tów narysowaæ
		word min_ind; // odpowiednik parametru DrawIndexedPrimitive - MinIndex (tylko wyra¿ony w trójk¹tach)
		word n_ind; // odpowiednik parametru DrawIndexedPrimitive - NumVertices (tylko wyra¿ony w trójk¹tach)
		string name;
		::Texture* tex, *tex_normal, *tex_specular;
		Vec3 specular_color;
		float specular_intensity;
		int specular_hardness;
		float normal_factor, specular_factor, specular_color_factor;

		static const uint MIN_SIZE = 10;
	};

	struct BoneGroup
	{
		word parent;
		string name;
		vector<byte> bones;

		static const uint MIN_SIZE = 4;
	};

	struct Bone
	{
		word id;
		word parent;
		Matrix mat;
		string name;
		vector<word> childs;

		static const uint MIN_SIZE = 51;
	};

	struct KeyframeBone
	{
		Vec3 pos;
		Quat rot;
		float scale;

		void Mix(Matrix& out, const Matrix& mul) const;
		static void Interpolate(KeyframeBone& out, const KeyframeBone& k, const KeyframeBone& k2, float t);

		static const KeyframeBone Zero;
	};

	struct Keyframe
	{
		float time;
		vector<KeyframeBone> bones;
	};

	struct Animation
	{
		string name;
		float length;
		word n_frames;
		vector<Keyframe> frames;

		static const uint MIN_SIZE = 7;

		int GetFrameIndex(float time, bool& hit);
		void GetKeyframeData(uint bone, float time, KeyframeBone& keyframe);
	};

	struct Point
	{
		enum Type : word
		{
			OTHER,
			SPHERE,
			Box
		};

		string name;
		Matrix mat;
		Vec3 rot;
		word bone;
		Type type;
		Vec3 size;

		static const uint MIN_SIZE = 73;

		bool IsSphere() const { return type == SPHERE; }
		bool IsBox() const { return type == Box; }
	};

	Mesh();
	~Mesh();
	void SetupBoneMatrices();
	Animation* GetAnimation(cstring name);
	Bone* GetBone(cstring name);
	Point* GetPoint(cstring name);

	Header head;
	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	vector<Submesh> subs;
	vector<Bone> bones;
	vector<Animation> anims;
	vector<Point> attach_points;
	vector<BoneGroup> groups;
	vector<Matrix> model_to_bone;
};
