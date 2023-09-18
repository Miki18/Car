#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Atrybuty
layout (location=0) in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
layout (location=1) in vec4 normal; //wektor normalny w wierzcholku
layout (location=2) in vec2 texCoord; //wspó³rzêdne teksturowania

//Zmienne interpolowane
out vec2 i_tc;
out float i_nl;

void main(void) {
     vec4 lp = vec4(160,5,-6,1);  //zmieniona lokalizacja jednego zrodla swiatla
     vec4 lp_1 = vec4(-2,5,6,1);

    //mat4 G=mat4(inverse(transpose(mat3(M))));
    //vec4 n=normalize(G*normal);

    vec4 l_1 = normalize(V*lp_1 - V*M*vertex) * 5;
    vec4 l = normalize(V*lp - V*M*vertex) * 5;
    vec4 n_c = normalize(V*M*normal);

    i_nl=clamp(dot(n_c,l),0,1) + clamp(dot(n_c, l_1),0,1);  //clamp (something, min value, max value)
    i_tc=texCoord;     //koordynaty texturki

    gl_Position=P*V*M*vertex;
}