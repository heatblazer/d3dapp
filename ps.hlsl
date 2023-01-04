
/* vertex attributes go here to input to the vertex shader */
struct vs_in {
    float3 position_local : POS;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out {
    float4 position_clip : SV_POSITION; // required output of VS
};

float4 ps_main(vs_out input) : SV_TARGET{
  return float4(0.0, 0.0, 1.0, 1.0); // must return an RGBA colour
}