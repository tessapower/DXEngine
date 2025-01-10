cbuffer c_buffer {
    float4 face_colors[6];
};

float4 main(uint triangle_id : SV_PrimitiveID) : SV_Target {
    return face_colors[triangle_id / 2];
}