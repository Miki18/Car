#version 330


uniform sampler2D tex;   //textura

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

//Zmienne interpolowane
in float i_nl;   //oswietlenie
in vec2 i_tc;   //coordynaty

in vec4 v;
in vec4 n;

void main(void) {
	//pixelColor=texture(tex,i_tc);    //pixel kolorki
	vec4 color=texture(tex,i_tc);   //obliczanie oœwietlenia
	pixelColor=vec4(color.rgb*i_nl,color.a); 
}