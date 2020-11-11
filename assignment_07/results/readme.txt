## Computer Graphics 
## Exercise 07 - Shadows and Cube Mapping
## Date: 13.11.2020
## Team Members: T.Buchegger, C.Duran, N.Meinen
##############################################################################################################

• 20%: Building the projection matrix for rendering the shadow maps.
    Using the static parameters given.
• 30%: Building the view matrix for each face of the shadow cube map.
    Implement the view according to the 6 faces of the cube.
• 10%: Setting the blending mode so that each diffuse/specular lighting pass adds to the output color.
    For each light we set the parameters for the shaders.
• 10%: Completing shadowmap_gen.frag to draw the distance map for a light.
    Compute the distance.
• 30%: Completing phong_shadow.frag to render the diffuse and specular contributions to unshaded fragments only.
    To reduce errors, we use a kind of margin. Here the hints said 1.01x: When used with multiplication, we saw artifacts.
    When added no artifacts visible.

