#pragma once
// FBX geometry analysis only. Include once at file scope; C++17, ufbx 0.23.1.
#include "ThirdParty/ufbx/ufbx.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace BistroLighting {
struct Vec3 { double x=0, y=0, z=0; };
inline Vec3 operator+(Vec3 a,Vec3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline Vec3 operator-(Vec3 a,Vec3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline Vec3 operator*(Vec3 a,double s){return {a.x*s,a.y*s,a.z*s};}
inline Vec3 operator/(Vec3 a,double s){return a*(1.0/s);}
inline double Dot(Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
inline double Length(Vec3 v){return std::sqrt(Dot(v,v));}
inline Vec3 Normalize(Vec3 v,Vec3 fallback={0,-1,0}){double l=Length(v);return l>1e-10?v/l:fallback;}
inline Vec3 Cross(Vec3 a,Vec3 b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
inline Vec3 V(ufbx_vec3 v){return {v.x,v.y,v.z};}
inline std::string S(ufbx_string s){return std::string(s.data,s.length);}
inline bool Has(const std::string& s,const std::string& p){return s.find(p)!=std::string::npos;}
inline std::string OwnerName(const ufbx_node* n){for(auto p=n;p;p=p->parent)if(p->name.length)return S(p->name);return "unnamed";}

struct LightSpec {
    std::string name, category, sourceNode, sourceMaterial, emissionTexture, rationale;
    uint32_t sourceNodeElementId=0, materialSlot=0, component=0;
    uint32_t type=0; // AlphaEngine: 0 point, 1 spot; never 2.
    Vec3 anchor, position, direction={0,-1,0}, color={1,1,1};
    float intensity=0, radius=1;
    // Half angles measured from the cone axis. GPU uses cos(inner/outer).
    // Point lights receive valid inert values too, but their shader ignores them.
    float innerHalfAngleDeg=25, outerHalfAngleDeg=45;
};
struct Options {
    float intensityScale=1.0f;
    size_t maxStreet=24, maxLantern=5, maxFixture=8, maxSign=2, maxWindow=4;
    size_t maxRoofSpot=3;
    size_t maxTotal=48;
    bool addWindowSpill=true;
    bool addStringLights=true;
    float stringIntensity=12.0f, stringRadius=3.0f; // Authored test settings, not FBX photometry.
};
struct Surface {
    const ufbx_node* node=nullptr; const ufbx_material* material=nullptr;
    uint32_t slot=0, component=0;
    Vec3 lo={1e30,1e30,1e30}, hi={-1e30,-1e30,-1e30};
    Vec3 center, normal, largestNormal; double area=0,largestArea=0;
};
struct Triangle { Vec3 p[3], normal; double area; uint32_t key[3]; };
struct UnionFind {
    std::vector<size_t> p;
    explicit UnionFind(size_t n):p(n){std::iota(p.begin(),p.end(),size_t(0));}
    size_t Find(size_t a){while(p[a]!=a){p[a]=p[p[a]];a=p[a];}return a;}
    void Join(size_t a,size_t b){a=Find(a);b=Find(b);if(a!=b)p[b]=a;}
};
inline void Extend(Surface& s,Vec3 p){s.lo={std::min(s.lo.x,p.x),std::min(s.lo.y,p.y),std::min(s.lo.z,p.z)};s.hi={std::max(s.hi.x,p.x),std::max(s.hi.y,p.y),std::max(s.hi.z,p.z)};}
inline std::vector<Surface> SplitSurface(const ufbx_node* node,const ufbx_mesh_part& part,double mergeDistance){
    const auto* mesh=node->mesh;
    if(!mesh||part.index>=node->materials.count)return {};
    std::vector<Triangle> ts;std::vector<uint32_t> indices(mesh->max_face_triangles*3);
    using Key=std::tuple<int64_t,int64_t,int64_t>;
    std::map<Key,uint32_t> weld;
    const auto nm=ufbx_matrix_for_normals(&node->geometry_to_world);
    for(uint32_t fi:part.face_indices){
        uint32_t count=ufbx_triangulate_face(indices.data(),indices.size(),mesh,mesh->faces.data[fi]);
        for(size_t i=0;i<size_t(count)*3;i+=3){
            Triangle t{};Vec3 sn{};
            for(size_t j=0;j<3;j++){
                uint32_t c=indices[i+j];
                t.p[j]=V(ufbx_transform_position(&node->geometry_to_world,ufbx_get_vertex_vec3(&mesh->vertex_position,c)));
                auto p=t.p[j];Key key{int64_t(std::llround(p.x*10000)),int64_t(std::llround(p.y*10000)),int64_t(std::llround(p.z*10000))};
                auto it=weld.emplace(key,uint32_t(weld.size())).first;t.key[j]=it->second;
                if(mesh->vertex_normal.exists)sn=sn+V(ufbx_transform_direction(&nm,ufbx_get_vertex_vec3(&mesh->vertex_normal,c)));
            }
            Vec3 cr=Cross(t.p[1]-t.p[0],t.p[2]-t.p[0]);t.area=Length(cr)*.5;
            t.normal=Normalize(sn,Normalize(cr));if(t.area>1e-12)ts.push_back(t);
        }
    }
    UnionFind uf(weld.size());for(auto& t:ts){uf.Join(t.key[0],t.key[1]);uf.Join(t.key[0],t.key[2]);}
    std::map<size_t,Surface> groups;
    for(auto& t:ts){auto& s=groups[uf.Find(t.key[0])];s.node=node;s.material=node->materials.data[part.index];s.slot=part.index;
        for(auto p:t.p)Extend(s,p);
        s.area+=t.area;s.center=s.center+(t.p[0]+t.p[1]+t.p[2])*(t.area/3);s.normal=s.normal+t.normal*t.area;
        if(t.area>s.largestArea){s.largestArea=t.area;s.largestNormal=t.normal;}
    }
    std::vector<Surface> a;for(auto& kv:groups){auto s=kv.second;s.center=s.center/s.area;a.push_back(s);}
    // Merge only nearby disconnected surfaces of the same material/fixture.
    // Never average distant lamps merely because they share one mesh.
    UnionFind merge(a.size());for(size_t i=0;i<a.size();i++)for(size_t j=0;j<i;j++)if(Length(a[i].center-a[j].center)<mergeDistance)merge.Join(i,j);
    std::map<size_t,Surface> joined;
    for(size_t i=0;i<a.size();i++){auto& d=joined[merge.Find(i)];auto& s=a[i];d.node=node;d.material=s.material;d.slot=s.slot;Extend(d,s.lo);Extend(d,s.hi);d.area+=s.area;d.center=d.center+s.center*s.area;d.normal=d.normal+s.normal;if(s.largestArea>d.largestArea){d.largestArea=s.largestArea;d.largestNormal=s.largestNormal;}}
    a.clear();for(auto& kv:joined){auto s=kv.second;s.center=s.center/s.area;s.normal=Normalize(s.normal, s.largestNormal);a.push_back(s);}
    std::sort(a.begin(),a.end(),[](auto& a,auto& b){return std::tie(a.center.x,a.center.y,a.center.z)<std::tie(b.center.x,b.center.y,b.center.z);});
    for(size_t i=0;i<a.size();i++)a[i].component=uint32_t(i);
    return a;
}
inline LightSpec Base(const Surface& s,const char* category,uint32_t type){
    LightSpec l;l.category=category;l.type=type;l.sourceNode=OwnerName(s.node);l.sourceNodeElementId=s.node->element_id;l.sourceMaterial=S(s.material->name);l.materialSlot=s.slot;l.component=s.component;l.anchor=s.center;l.position=s.center;
    l.name=l.category+"/"+l.sourceNode+"/"+std::to_string(s.slot)+"/"+std::to_string(s.component);
    if(auto* t=s.material->pbr.emission_color.texture)l.emissionTexture=S(t->relative_filename);
    return l;
}
inline std::vector<LightSpec> BuildBase(const ufbx_scene* scene,const Options& opt={}){
    if(!scene)throw std::invalid_argument("BistroLighting: null FBX scene");
    if(opt.intensityScale<0 || !std::isfinite(opt.intensityScale))throw std::invalid_argument("BistroLighting: non-finite intensityScale");
    // Caller must use the same +X,+Y,+Z target axes and target_unit_meters=1 as TestLoader.
    std::vector<LightSpec> street,lantern,fixture,sign,windows;std::vector<Surface> glassWindows;std::vector<Vec3> tables;
    for(auto* node:scene->nodes){
        if(!node->mesh)continue;
        const std::string owner=OwnerName(node);
        for(auto part:node->mesh->material_parts){
            if(!part.num_triangles||part.index>=node->materials.count)continue;
            const std::string mat=S(node->materials.data[part.index]->name);
            if(mat=="Paris_Table_Terrace"){
                // Table geometry is not itself a light. Used only to choose the outward side of nearby windows/signs.
                auto ss=SplitSurface(node,part,3.0);for(auto& s:ss)tables.push_back(s.center);continue;
            }
            if(mat=="Emissive_StreetLight"){
                bool hanging=Has(owner,"Lantern_Wind_");
                // Keep actual named street fixtures; small building bulbs are omitted by design.
                if(!hanging && !Has(owner,"StreetLight_Glass") && !Has(owner,"Streetlight_Glass"))continue;
                for(auto& s:SplitSurface(node,part,.65)){
                    auto l=Base(s,hanging?"Lantern":"Street",0);
                    l.position=(s.lo+s.hi)*.5;l.direction={0,-1,0};
                    l.color=hanging?Vec3{1,.67,.35}:Vec3{1,.79,.53};
                    l.intensity=hanging?22.f:45.f;l.radius=hanging?4.5f:8.5f;
                    l.rationale=hanging?"emissive lantern glass AABB center; existing fixture over terrace":"emissive street-lamp glass AABB center; one per measured fixture";
                    (hanging?lantern:street).push_back(l);
                }
            }else if(mat=="Spotlight_Glass_Emissive"){
                std::vector<Surface> bulbs;
                for(auto bp:node->mesh->material_parts)if(bp.index<node->materials.count&&S(node->materials.data[bp.index]->name)=="Spotlight_Emissive"){
                    auto b=SplitSurface(node,bp,.2);bulbs.insert(bulbs.end(),b.begin(),b.end());
                }
                for(auto& s:SplitSurface(node,part,.2)){
                    auto l=Base(s,"FixtureSpot",1);Vec3 dir=s.normal;
                    double best=1e30;Vec3 outward{};
                    for(auto& b:bulbs){double dist=Length(s.center-b.center);if(dist<best){best=dist;outward=s.center-b.center;}}
                    if(best<1&&Dot(dir,outward)<0)dir=dir*-1;
                    l.direction=Normalize(dir);l.position=s.center+l.direction*.10;
                    l.color=s.center.y>10?Vec3{1,.84,.62}:Vec3{1,.70,.40};
                    l.intensity=s.center.y>10?65.f:45.f;l.radius=s.center.y>10?13.f:9.f;
                    l.innerHalfAngleDeg=22;l.outerHalfAngleDeg=38;
                    l.rationale="separate lens connected component; authored surface normal oriented from inner bulb toward lens; offset 0.10m";
                    fixture.push_back(l);
                }
            }else if(mat=="Shopsign_Pharmacy_Emissive"||mat=="Shopsign_Book_Store_Emissive"){
                for(auto& s:SplitSurface(node,part,1.5)){
                    auto l=Base(s,"Sign",0);l.color=Has(mat,"Pharmacy")?Vec3{.25,1,.45}:Vec3{1,.72,.42};
                    l.intensity=Has(mat,"Pharmacy")?12.f:10.f;l.radius=3.2f;
                    l.rationale="emissive sign geometry; authored color accent (not sampled from DDS)";sign.push_back(l);
                }
            }else if(opt.addWindowSpill&&mat=="MASTER_Glass_Exterior"&&Has(owner,"building_01_bottom")){
                for(auto& s:SplitSurface(node,part,.15))if(s.area>.8&&s.center.y>.8&&s.center.y<5)glassWindows.push_back(s);
            }
        }
    }
    // Window materials are not emissive: explicitly authored shop-interior spill proxies.
    // Normal sign is selected toward the nearest real terrace table (within 8m).
    for(auto& s:glassWindows){
        Vec3 toward{};double best=1e30;
        for(auto p:tables){double d=Length(p-s.center);if(d<best){best=d;toward=p-s.center;}}
        if(best>8)continue;
        Vec3 normal=Normalize({s.normal.x,0,s.normal.z},{0,0,1});if(Dot(normal,toward)<0)normal=normal*-1;
        auto l=Base(s,"WindowSpill",1);l.position=s.center+normal*.18;
        l.direction=Normalize(normal+Vec3{0,-.20,0});l.color={1,.63,.32};l.intensity=16;l.radius=5.0;
        l.innerHalfAngleDeg=30;l.outerHalfAngleDeg=55;
        l.rationale="non-emissive exterior glass; authored warm shop spill 0.18m outside pane, oriented toward nearest terrace table";windows.push_back(l);
    }
    // Prefer the active terrace neighborhood instead of truncating in FBX traversal order.
    auto tableDistance=[&](const LightSpec& l){
        double d=1e30;for(auto p:tables){auto delta=l.position-p;delta.y=0;d=std::min(d,Dot(delta,delta));}return d;
    };
    auto nearer=[&](const LightSpec& a,const LightSpec& b){double da=tableDistance(a),db=tableDistance(b);return da!=db?da<db:a.name<b.name;};
    std::sort(street.begin(),street.end(),nearer);
    // Three separated existing upward-facing rooftop lenses, then real terrace-facing fixtures.
    std::vector<LightSpec> roof,down,curated;
    for(auto l:fixture)(l.position.y>10 && l.direction.y>0?roof:down).push_back(l);
    std::sort(roof.begin(),roof.end(),[](auto& a,auto& b){return a.name<b.name;});
    for(auto l:roof){bool near=false;for(auto& q:curated)if(Length(l.position-q.position)<8)near=true;
        if(!near && curated.size()<std::min(opt.maxRoofSpot,opt.maxFixture))curated.push_back(l);}
    std::sort(down.begin(),down.end(),nearer);
    for(auto l:down){bool near=false;for(auto& q:curated)if(Length(l.position-q.position)<3.5)near=true;
        if(!near && curated.size()<opt.maxFixture)curated.push_back(l);}
    fixture=std::move(curated);
    std::sort(lantern.begin(),lantern.end(),[](auto& a,auto& b){return a.name<b.name;});
    std::sort(sign.begin(),sign.end(),[](auto& a,auto& b){return a.name<b.name;});
    std::sort(windows.begin(),windows.end(),[](auto& a,auto& b){return a.name<b.name;});
    // Signed lenses, panes and duplicate back/front FBX meshes must not double the light count.
    std::vector<LightSpec> out;
    auto append=[&](std::vector<LightSpec>& v,size_t cap,double spacing){
        size_t added=0;for(auto l:v){
            if(added>=cap||out.size()>=opt.maxTotal)break;
            bool duplicate=false;for(auto& q:out)if(q.category==l.category&&Length(q.position-l.position)<spacing){duplicate=true;break;}
            if(duplicate)continue;
            l.intensity*=opt.intensityScale;out.push_back(l);added++;
        }
    };
    append(street,opt.maxStreet,.6);append(lantern,opt.maxLantern,.6);append(fixture,opt.maxFixture,.5);append(sign,opt.maxSign,1.5);append(windows,opt.maxWindow,3.0);
    return out;
}

// Measured uniform mip-0 BaseColor DDS RGB8 values. No DDS dependency at runtime.
// Decode sRGB then normalize peak to one, keeping hue separate from intensity.
inline Vec3 StringBulbColor(const std::string& material) {
    Vec3 rgb;
    if(material=="Paris_StringLights_01_Blue_Color_Emissive") rgb={0, 74, 255}; else
    if(material=="Paris_StringLights_01_Red_Color_Emissive") rgb={192, 1, 0}; else
    if(material=="Paris_StringLights_01_Green_Color_Emissive") rgb={0, 146, 8}; else
    if(material=="Paris_StringLights_01_White_Color_Emissive") rgb={255, 255, 255}; else
    if(material=="Paris_StringLights_01_Pink_Color_Emissive") rgb={211, 8, 255}; else
    if(material=="Paris_StringLights_01_Orange_Color_Emissive") rgb={247, 175, 74}; else
    throw std::runtime_error("Unmeasured StringLight material: " + material);
    auto linear=[](double x){x/=255.;return x<=.04045?x/12.92:std::pow((x+.055)/1.055,2.4);};
    rgb={linear(rgb.x),linear(rgb.y),linear(rgb.z)};
    return rgb/std::max({rgb.x,rgb.y,rgb.z});
}
inline std::vector<LightSpec> BuildStringLights(const ufbx_scene* scene,const Options& opt={}) {
    if(!scene) throw std::invalid_argument("Null scene");
    if(!std::isfinite(opt.intensityScale)||opt.intensityScale<0 || !std::isfinite(opt.stringIntensity)||opt.stringIntensity<0 || !std::isfinite(opt.stringRadius)||opt.stringRadius<=0)
        throw std::invalid_argument("Invalid string light parameters");
    std::vector<LightSpec> out;
    if(!opt.addStringLights)return out;
    for(auto* node:scene->nodes){
        if(!node->mesh)continue;
        bool hasStringMaterial=false;
        for(auto* mat:node->materials)if(S(mat->name)=="Stringlights")hasStringMaterial=true;
        if(!hasStringMaterial)continue;
        for(auto part:node->mesh->material_parts){
            if(!part.num_triangles||part.index>=node->materials.count)continue;
            const std::string mat=S(node->materials.data[part.index]->name);
            if(!Has(mat,"Paris_StringLights_01_")||!Has(mat,"_Color_Emissive"))continue;
            for(auto& s:SplitSurface(node,part,0.0)){
                auto l=Base(s,"StringBulb",0);
                l.position=(s.lo+s.hi)*.5;
                l.color=StringBulbColor(mat);
                l.intensity=opt.stringIntensity*opt.intensityScale;l.radius=opt.stringRadius;
                l.rationale="one connected bulb surface, no distance merge or count cap; world AABB center; measured BaseColor sRGB converted to linear, peak-normalized";
                out.push_back(l);
            }
        }
    }
    std::sort(out.begin(),out.end(),[](const auto& a,const auto& b){return a.name<b.name;});
    return out;
}
inline std::vector<LightSpec> Build(const ufbx_scene* scene,const Options& opt={}) {
    auto out=BuildBase(scene,opt);
    auto strings=BuildStringLights(scene,opt);
    // maxTotal and previous category caps apply ONLY to the original rig.
    // Every measured string bulb is appended, without performance truncation.
    out.insert(out.end(),strings.begin(),strings.end());
    return out;
}
} // namespace BistroLighting
