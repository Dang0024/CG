#version 150

in vec2 pass_TexCoord;  // texture coordinates

out vec4 out_Color;

// uniform values
uniform bool HorizontalMirror;
uniform bool VerticalMirror;
uniform sampler2D ColorTex; // sampler variable

void main() {

    float x = pass_TexCoord.x;
    float y = pass_TexCoord.y;

    // Horizontally Mirrored
    if(HorizontalMirror)
        y = abs( 1.0f - y );

    // Vertically Mirrored
    if(VerticalMirror)
        x = abs( 1.0f - x );      

    out_Color = texture(ColorTex, vec2(x, y));

}
