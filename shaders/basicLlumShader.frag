#version 330 core
in vec3 color;
out vec4 FragColor;


in vec3 vertexOut;
in vec3 normalOut;
uniform mat3 NormalMatrix;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;
uniform vec4 luces[8];
uniform vec3 HSVColor;
in vec3 matambOut;
in vec3 matspecOut;
in vec3 matdiffOut;
in float matshinOut;
in vec3 NormSCO;
in vec4 ver;
uniform float newAlpha;

in vec3 fcolor;
vec3 colFocus = HSVColor;
vec3 llumAmbient = vec3(0.1, 0.1, 0.1);
vec3 posFocus = vec3(0, 7, 5);  // en SCA


vec3 Lambert (vec3 NormSCO, vec3 L, vec3 colLuz)
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats
    vec3 colRes = vec3(0.0,0.0,0.0);

    // Inicialitzem color a component ambient
    //vec3 colRes = llumAmbient * matambOut;


    // Afegim component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colLuz * matdiffOut * dot (L, NormSCO);
    return (colRes);
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO, vec3 colLuz)
{
    // Els vectors estan normalitzats
    vec3 colRes = vec3(0.0,0.0,0.0);
    // Inicialitzem color a Lambert
    //vec3 colRes = Lambert (NormSCO, L);

    // Calculem R i V
    if ((dot(NormSCO,L) < 0) || (matshinOut == 0))
      return colRes;  // no hi ha component especular

    vec3 R = reflect(-L, NormSCO); // equival a: 2.0*dot(NormSCO,L)*NormSCO - L;
    vec3 V = normalize(-vertSCO.xyz); // perquè la càmera està a (0,0,0) en SCO

    if (dot(R, V) < 0)
      return colRes;  // no hi ha component especular

    float shine = pow(max(0.0, dot(R, V)), matshinOut);
    return (colRes + matspecOut * colLuz * shine);
}

void main() {

    vec3 NormalizedSCO = normalize(normalOut);
    //luz ambient
    vec4 L  = normalize(view * vec4(posFocus,1.0) - ver);
    vec3 colorLuz = llumAmbient * matambOut;
    colorLuz+= Lambert(NormalizedSCO,L.xyz,HSVColor);
    colorLuz+= Phong(NormalizedSCO,L.xyz,ver,HSVColor);

    //para las luces ovni
    vec3 colVentana = vec3(0.3,0.3,0.3);
    for(int i=0; i<8;i++){
        L  = normalize(view * luces[i] - ver);
        colorLuz+= Lambert(NormalizedSCO,L.xyz,colVentana);
        colorLuz+= Phong(NormalizedSCO,L.xyz,ver,colVentana);
    }
    FragColor = vec4(colorLuz,newAlpha);

}



