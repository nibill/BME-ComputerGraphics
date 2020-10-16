## Computer Graphics 
## Exercise 03 - Ray Tracing Meshes
## Date: 16.10.2020
## Team Members: T.Buchegger, C.Duran, N.Meinen
##############################################################################################################

Vertex Normals
##############
Considering the given code. I think the solution is already given, as the Vertex normals are already normalized with v.normal = normalize(v.normal);

Ray-triangle Intersection
#########################
The most difficult part was to find an elegant solution for the determinant. Then also considering the double errors. To satisfy alpha + beta + gamma = 1, I must add a slight margin to not have "black dots" in the mask.


Efficient Ray-mesh Intersections
################################
I had to remember that we keep the min-max for all dimensions and not evaluate min-max per dimension.