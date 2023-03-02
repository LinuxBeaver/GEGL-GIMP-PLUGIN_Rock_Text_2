/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2023 Beaver (GEGL Rock Text 2) 
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES



property_int    (amountx, _("Rockification Horizontal"), 30)
    description (_("Horizontal spread amount"))
    value_range (0, 80)
    ui_meta     ("unit", "pixel-distance")
    ui_meta     ("axis", "x")

property_int    (amounty, _("Rockification Vertical"), 22)
    description (_("Vertical spread amount"))
    value_range (0, 80)
    ui_meta     ("unit", "pixel-distance")
    ui_meta     ("axis", "y")

property_seed (seed, _("Random seed"), rand)


property_string (string1, _("Graph1"), TUTORIAL1)
    ui_meta     ("role", "output-extent")

#define TUTORIAL1 \
" gimp:threshold-alpha bloom strength=10 "\


property_string (string2, _("Graph2"), TUTORIAL2)
    ui_meta     ("role", "output-extent")

#define TUTORIAL2 \
" crop denoise-dct patch-size=size8x8 sigma=8 id=slemboss gimp:layer-mode opacity=0.50 layer-mode=softlight aux=[ ref=slemboss emboss depth=10 elevation=60 azimuth=120 ] noise-reduction iterations=2 "\



enum_start (gegl_blend_mode_typecbevel3)
  enum_value (GEGL_BLEND_MODE_TYPE_HARDLIGHT3, "Hardlight",
              N_("HardLight"))
  enum_value (GEGL_BLEND_MODE_TYPE_MULTIPLY3,      "Multiply",
              N_("Multiply"))
  enum_value (GEGL_BLEND_MODE_TYPE_COLORDODGE3,      "ColorDodge",
              N_("ColorDodge"))
  enum_value (GEGL_BLEND_MODE_TYPE_PLUS3,      "Plus",
              N_("Plus"))
  enum_value (GEGL_BLEND_MODE_TYPE_DARKEN3,      "Darken",
              N_("Darken"))
  enum_value (GEGL_BLEND_MODE_TYPE_LIGHTEN3,      "Lighten",
              N_("Lighten"))
  enum_value (GEGL_BLEND_MODE_TYPE_OVERLAY3,      "Overlay",
              N_("Overlay"))
  enum_value (GEGL_BLEND_MODE_TYPE_GRAINMERGE3,      "GrainMerge",
              N_("Grain Merge"))
  enum_value (GEGL_BLEND_MODE_TYPE_SOFTLIGHT3,      "Softlight",
              N_("Soft Light"))
  enum_value (GEGL_BLEND_MODE_TYPE_ADDITION3,      "Addition",
              N_("Addition"))
  enum_value (GEGL_BLEND_MODE_TYPE_EMBOSSBLEND3,      "EmbossBlend",
              N_("ImageandColorOverlayMode"))
enum_end (GeglBlendModeTypecbevel3)

property_enum (blendmode3, _("Blend Mode of Internal Emboss"),
    GeglBlendModeTypecbevel3, gegl_blend_mode_typecbevel3,
    GEGL_BLEND_MODE_TYPE_HARDLIGHT3)


property_int (depth, _("Bevel Depth (makes darker)"), 100)
    description (_("Filter width"))
    value_range (10, 100)

property_double (gaus, _("Internal Gaussian Blur (fatness of bevel)"), 5)
   description (_("Standard deviation for the XY axis"))
   value_range (0.0, 5.0)



property_int (box, _("Internal Box Blur (fatness of bevel)"), 1)
   description(_("Radius of square pixel region, (width and height will be radius*2+1)"))
   value_range (0, 6)
   ui_range    (0, 6)
   ui_gamma   (1.5)




property_int  (radius, _("Radius (Lower makes wider)"), 2)
  value_range (-40, 40)
  ui_range    (-40, 40)
  ui_meta     ("unit", "pixel-distance")
  description (_("Neighborhood radius, a negative value will calculate with inverted percentiles"))


property_double  (alpha_percentile, _("Median's Alpha Percentile (Wideness control)"), 63)
  value_range (25, 75)
  description (_("Neighborhood alpha percentile"))

property_double  (alphapercentile, _("Median's Alpha Percentile 2 (Wideness control)"), 10)
  value_range (10, 75)
  description (_("Neighborhood alpha percentile"))

property_color (coloroverlay, _("Color Overlay (uses Multiply blend mode)"), "#ffffff")
    description (_("The color to paint over the input"))

property_double (inlow, _("Lightness to Darkness meter"), 0.09)
    description ( _("Input luminance level to become lowest output"))
    ui_range    (0.0, 0.25)



property_file_path(layer, _("Image file overlay (color white recommended)"), "")
    description (_("Source image file path (png, jpg, raw, svg, bmp, tif, ...)"))

property_double (angle, _("Angle of Extrusion"), 72.0)
  description (_("Shadow angle"))
  value_range (-180.0, 180.0)
  ui_meta     ("unit", "degree")
  ui_meta     ("direction", "cw")

property_double (length, _("Length of Extrusion"), 23)
  description (_("Shadow length"))
  value_range (0, 55)
  ui_range    (0, 55)




property_double (lightness, _("Darkness to Lightness of Extrusion"), 2.4)
   description  (_("Lightness adjustment"))
   value_range  (-30.0, 30.0)




#else

#define GEGL_OP_META
#define GEGL_OP_NAME     rocktext2
#define GEGL_OP_C_SOURCE rocktext2.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *median, *spread, *ta, *cb, *nop, *graph1, *levels, *behind, *lspd, *inlow, *color, *multiply, *graph2, *output;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");



  cb    = gegl_node_new_child (gegl,
                                  "operation", "gegl:custom-bevel",
                                  NULL);

  lspd    = gegl_node_new_child (gegl,
                                  "operation", "gegl:long-shadow-pd",
                                  NULL);

  color    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

  median    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);

  multiply    = gegl_node_new_child (gegl,
                                  "operation", "gegl:multiply",
                                  NULL);

  spread    = gegl_node_new_child (gegl,
                                  "operation", "gegl:noise-spread",
                                  NULL);

  behind    = gegl_node_new_child (gegl,
                                  "operation", "gegl:dst-atop",
                                  NULL);

  graph1    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl",
                                  NULL);

  graph2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl",
                                  NULL);

  ta    = gegl_node_new_child (gegl,
                                  "operation", "gimp:threshold-alpha",
                                  NULL);

  nop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

  levels    = gegl_node_new_child (gegl,
                                  "operation", "gegl:levels",
                                  NULL);
                        
                        
                        
                        
                        
                        
  gegl_operation_meta_redirect (operation, "value", color, "value");
  gegl_operation_meta_redirect (operation, "gaus", cb, "gaus");
  gegl_operation_meta_redirect (operation, "box", cb, "box");
  gegl_operation_meta_redirect (operation, "depth", cb, "depth");
  gegl_operation_meta_redirect (operation, "alphapercentile", cb, "alphapercentile");
  gegl_operation_meta_redirect (operation, "coloroverlay", cb, "coloroverlay");
  gegl_operation_meta_redirect (operation, "blendmode3", cb, "blendmode");
  gegl_operation_meta_redirect (operation, "radius", median, "radius");
  gegl_operation_meta_redirect (operation, "alpha_percentile", median, "alpha-percentile");
  gegl_operation_meta_redirect (operation, "amountx", spread, "amount-x");
  gegl_operation_meta_redirect (operation, "amounty", spread, "amount-y");
  gegl_operation_meta_redirect (operation, "seed", spread, "seed");
  gegl_operation_meta_redirect (operation, "layer", cb, "src");
  gegl_operation_meta_redirect (operation, "string1", graph1, "string");
  gegl_operation_meta_redirect (operation, "string2", graph2, "string");
  gegl_operation_meta_redirect (operation, "angle", lspd, "angle");
  gegl_operation_meta_redirect (operation, "length", lspd, "length");
  gegl_operation_meta_redirect (operation, "lightness", lspd, "lightness");
  gegl_operation_meta_redirect (operation, "inlow", levels, "in-low");




  gegl_node_link_many (input, median, spread, ta, cb, graph1, nop, behind, graph2, levels, output, NULL);
  gegl_node_link_many (nop, lspd, NULL);
  gegl_node_connect_from (behind, "aux", lspd, "output");


}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:rock-text2",
    "title",       _("Rock Text 2"),
    "categories",  "Aristic",
    "reference-hash", "b2do6a2ro26rogk0fjf25sb2ac",
    "description", _("GEGL generates specialized beveled text like a rock. Filter Requires the user to use their image file overlay texture. Textures provided in rock text repo. This filter has two required dependency filters; custom-bevel and extrusion 2. Both are useful filters on their own and are required for rock text 2 to work. "
                     ""),
    NULL);
}

#endif
