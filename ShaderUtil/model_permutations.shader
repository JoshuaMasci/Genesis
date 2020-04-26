{
  "output_name_pre": "model_",
  "output_name_post": ".frag",
  "file_header": "model_header.slib",
  "file_tail": "model_tail.slib",
  "elements": [
    "color",
    "normal",
    "uv",
    "light"
  ],
  "color": {
    "albedo": "color_albedo.slib",
    "texture": "color_texture.slib"
  },
  "normal": {
    "basic": "normal_basic.slib",
    "mapped": "normal_mapped.slib"
  },
  "uv": {
    "basic": "uv_basic.slib",
    "parallax": "uv_parallax.slib",
    "parallax_occlusion": "uv_parallax_occlusion.slib"
  },
  "light": {
    "ambient": "light_ambient.slib",
    "directional": "light_directional.slib",
    "point": "light_point.slib",
    "spot": "light_spot.slib",
    "directional_shadow": "light_directional_shadow.slib",
    "point_shadow": "light_point_shadow.slib",
    "spot_shadow": "light_spot_shadow.slib"
  }
}