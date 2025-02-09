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

/*

GEGL Graph of Rock Text 2 so users can test without installing. REQUIRES LONG SHADOW PD AND CUSTOM BEVEL

#rock text2
median-blur radius=2 alpha-percentile=80
noise-spread amount-x=38 amount-y=48 seed=233
gimp:threshold-alpha
custom-bevel blendmode=Hardlight  gaus=4 box=4 alphapercentile=10
gimp:threshold-alpha
bloom strength=10
id=ls
gimp:layer-mode layer-mode=behind aux=[ ref=ls lb:long-shadow-pd angle=90 length=9 lightness=99  ]
crop
denoise-dct patch-size=size8x8 sigma=7
id=slemboss
gimp:layer-mode opacity=0.50 layer-mode=softlight aux=[ ref=slemboss emboss depth=10 elevation=60 azimuth=120 ]
levels in-low=0.09
brightness-contrast brightness=0.04
noise-reduction iterations=2
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

#define TUTORIAL1 \
" gimp:threshold-alpha bloom strength=10 "\


#define TUTORIAL2 \
" id=1 crop aux=[ ref=1 ] mean-curvature-blur iterations=4 id=slemboss gimp:layer-mode opacity=0.50 layer-mode=softlight aux=[ ref=slemboss emboss depth=10 elevation=60 azimuth=120 ] noise-reduction iterations=2 "\

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


property_int (depth, _("Bevel Depth and or Detail"), 100)
    description (_("Depending on the blend mode this will either add depth and or bring out details"))
    value_range (10, 100)

property_double (gaus, _("Internal Gaussian Blur (fatness of bevel)"), 5)
   description (_("Gaussian Blur makes a normal fat bevel"))
   value_range (0.0, 5.0)



property_int (box, _("Internal Box Blur (makes sharper bevel)"), 1)
   description(_("Box Blur makes a sharper bevel"))
   value_range (0, 6)
   ui_range    (0, 6)
   ui_gamma   (1.5)

property_int  (radius, _("Median's Radius"), 2)
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
    description (_("The color to paint over the rock text. If the color is white or grayscale this will change it to any color"))

property_double (inlow, _("Lightness to Darkness meter"), 0.09)
    description ( _("Levels in Low for lightness"))
    ui_range    (0.0, 0.25)



property_file_path(layer, _("Image file overlay (color white recommended)"), "")
    description (_("Source image file path (png, jpg, raw, svg, bmp, tif, ...)"))

property_double (angle, _("Angle of Extrusion"), 72.0)
  description (_("LongShadow angle"))
  value_range (-180.0, 180.0)
  ui_meta     ("unit", "degree")
  ui_meta     ("direction", "cw")

property_double (length, _("Length of Extrusion"), 23)
  description (_("LongShadow length"))
  value_range (0, 55)
  ui_range    (0, 55)


property_double (lightness, _("Darkness to Lightness of Extrusion"), 2.4)
   description  (_("Lightness adjustment for the LongShadow"))
   value_range  (-30.0, 30.0)

#else

#define GEGL_OP_META
#define GEGL_OP_NAME     rocktext2
#define GEGL_OP_C_SOURCE rocktext2.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *median, *spread, *ta, *cb, *nop, *graph1, *levels, *multiply, *behind, *lspd,  *color,  *graph2, *output;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");



  cb    = gegl_node_new_child (gegl,
                                  "operation", "lb:custom-bevel",
                                  NULL);

  lspd    = gegl_node_new_child (gegl,
                                  "operation", "lb:long-shadow-pd",
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
                                  "operation", "gegl:gegl", "string", TUTORIAL1,
                                  NULL);

  graph2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string", TUTORIAL2,
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
  gegl_operation_meta_redirect (operation, "angle", lspd, "angle");
  gegl_operation_meta_redirect (operation, "length", lspd, "length");
  gegl_operation_meta_redirect (operation, "lightness", lspd, "lightness");
  gegl_operation_meta_redirect (operation, "inlow", levels, "in-low");

  gegl_node_link_many (input, median, spread, ta, cb, graph1, nop, behind, graph2, levels, multiply, output, NULL);
  gegl_node_link_many (nop, lspd, NULL);
  gegl_node_connect (behind, "aux", lspd, "output");
  gegl_node_connect (multiply, "aux", color, "output");



}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:rock-text2",
    "title",       _("Rock Text 2"),
    "categories",  "Aristic",
    "reference-hash", "b2do6a2ro26rogk0fjf25sb2ac",
    "description", _("GEGL generates specialized beveled text like a rock. Filter HIGHLY RECOMMENDS the user to use their image file overlay texture. Textures provided in stone text 2 repo. This filter has two required dependency filters; custom-bevel and extrusion 2. Both are useful filters on their own and are required for rock text 2 to work. "
                     ""),
    "gimp:menu-path", "<Image>/Filters/Text Styling",
    "gimp:menu-label", _("Rock Text 2..."),
    NULL);
}

#endif
