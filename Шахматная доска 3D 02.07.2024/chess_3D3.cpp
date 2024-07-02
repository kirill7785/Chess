// GUI for chess. Внимание должно быть не более 1500 строк кода чтобы
// влезло в chad gpt. 471 строк кода 26.06.2024. Цель: нарисовать в 3D FEN строку.

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "GL/glut.h"
#include <math.h>
#include <fstream> // Для работы с файлом
#include <iostream>
#include <string>

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
float zoom = -13.0f; // Переменная для управления зумом
int lastMouseX, lastMouseY;

float A=1.0f;
float B=1.0f;
float C=1.0f;

enum class ChessPiece {EMPTY=0, W_KING = 1, W_QUEEN = 2, W_ROOK = 3, W_BISHOP = 4,  W_KNIGHT = 5,   W_PAWN=  6,
                                B_KING = 7, B_QUEEN = 8, B_ROOK = 9, B_BISHOP = 10, B_KNIGHT = 11, B_PAWN = 12};

// точка в трёхмерном пространстве
typedef struct TPOINT_FLOAT {
	float x, y, z;
	TPOINT_FLOAT() {
		x = 0.0f, y = 0.0f, z = 0.0f;
	}
} TOCHKA_FLOAT;

// CAD STL кубик состоит из набора треугольников.
// Тип одного треугольника 13.11.2020
typedef struct TgCAD_STL {
	TOCHKA_FLOAT n;
	TOCHKA_FLOAT pa, pb, pc;
	TgCAD_STL* next;

	TgCAD_STL() {
		next = nullptr;
	}

} gCAD_STL;

typedef struct TRenderChessMan {
	  gCAD_STL* root;
	  float GLOBAL_MOVE_ALL_STL_IN_IMPORT_X=0.0f;
    float GLOBAL_MOVE_ALL_STL_IN_IMPORT_Y=0.0f;
    float GLOBAL_MOVE_ALL_STL_IN_IMPORT_Z=0.0f;

    // Конструктор.
    TRenderChessMan(float move_x, float move_y, float move_z) {
		    root=nullptr;
        GLOBAL_MOVE_ALL_STL_IN_IMPORT_X = move_x;
        GLOBAL_MOVE_ALL_STL_IN_IMPORT_Y = move_y;
        GLOBAL_MOVE_ALL_STL_IN_IMPORT_Z = move_z;
	}

    // загрузка.
	// отрисовка.
	void draw(float r, float g, float b, int iaddx, int iaddy ) {
		if (root!=nullptr) {
		   glColor3f(r,g,b);

		   float scale = 1.0f;
		   float centerPosX=1.0f*iaddx;
		   float centerPosY=1.0f*iaddy;
		   float centerPosZ=0.0f;

		    gCAD_STL* tmp =root;
		    while (tmp!=nullptr) {
		        glBegin(GL_TRIANGLES); // отрисовка треугольника
		        glNormal3f(tmp->n.x, tmp->n.y, tmp->n.z);
		        glVertex3f(scale * tmp->pa.x + centerPosX, scale * tmp->pa.y + centerPosY, scale * tmp->pa.z + centerPosZ);
		        glVertex3f(scale * tmp->pb.x + centerPosX, scale * tmp->pb.y + centerPosY, scale * tmp->pb.z + centerPosZ);
		        glVertex3f(scale * tmp->pc.x + centerPosX, scale * tmp->pc.y + centerPosY, scale * tmp->pc.z + centerPosZ);
		        glEnd(); // конец отрисовки треугольника
		        tmp=tmp->next;
		    }
		}
	}

} RenderChessMan;

// Загружает фигуру в левый нижний угол в клетку a1.
RenderChessMan bishop(-0.31f-6.0f,-0.92f-1.0f,0.0f);
RenderChessMan castle(-0.3f-4.0f, -1.1f-2.0f, 0.0f);
RenderChessMan king(-0.20f-5.0f, -0.92f-1.0f, 0.0f);
RenderChessMan knight(0.0f-4.0f, -0.82f-1.0f, 0.0f);
RenderChessMan pawn(-0.41f-2.0f, -0.92f-1.0f, 0.0f);
RenderChessMan queen(-0.58f-2.0f, -0.92f-2.0f, 0.0f);

// Вычисляет нормаль к треугольнику.
/*void calculateNormal_for_triangle(TOCHKA_FLOAT& n, TOCHKA_FLOAT pa, TOCHKA_FLOAT pb, TOCHKA_FLOAT pc) {

  // Calculate the parameters for the plane
  n.x = (pb.y - pa.y) * (pc.z - pa.z) - (pb.z - pa.z) * (pc.y - pa.y);
  n.y = (pb.z - pa.z) * (pc.x - pa.x) - (pb.x - pa.x) * (pc.z - pa.z);
  n.z = (pb.x - pa.x) * (pc.y - pa.y) - (pb.y - pa.y) * (pc.x - pa.x);
  // Normalize vector n.
  float length_n = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
  n.x = n.x / length_n;
  n.y = n.y / length_n;
  n.z = n.z / length_n;
}*/

// Считывает бинарный STL файл с CAD геометрией.
// 15.11.2020; 17.11.2020 для большой .stl модели.
// 15.02.2023 записывает xS, yS, zS, xE yE, zE для каждого файла в специальный файл const_size_limit_stl.txt
void ReadSTL_binary(RenderChessMan &fig) {

  //std::cout << "please, input *.stl file name...\n";

  std::string FName;

  float scale_cad_import_data = 0.022f;// scal


  static int icstr = 0;
  // В файле add_line.txt при его наличии содержатся сеточные линии дополнительные
  // заданные пользователем вручную для модели.
  FILE* fp_name_stl_file = NULL;

  // создание файла для чтения.
#ifdef MINGW_COMPILLER
  fp_name_stl_file = fopen64("stl_name.txt", "r");
  int err_fp_name_stl_file = 0;
  if (fp_name_stl_file != NULL) {
    err_fp_name_stl_file = 0;
  }
  else {
    err_fp_name_stl_file = 1; // ошибка открытия.
  }
#else
  errno_t err_fp_name_stl_file;
  err_fp_name_stl_file = fopen_s(&fp_name_stl_file, "stl_name.txt", "r");
#endif



  //std::cin >> FName;
  char s[300];
  // Если в имени файла есть пробел то имя файла считается некорректно
  /*for (int i_7 = 0; i_7 <= icstr; ++i_7) {
    //fscanf_s(fp_name_stl_file, "%s", s, _countof(s));
    fscanf_s(fp_name_stl_file, "%s", s, static_cast<unsigned int>(sizeof(s)));
  }*/

  // 17.11.2022 учитывает пробелы в имени файла
  std::fstream inOut;
  inOut.open("stl_name.txt", std::ios::in);
  for (int i_7 = 0; i_7 <= icstr; ++i_7) {
    inOut.getline(s, 300);
  }
  inOut.close();

  FName = s;

  if ((err_fp_name_stl_file == 0)&&(fp_name_stl_file != NULL))
  {
    fclose(fp_name_stl_file);
  }
  icstr++;
  std::cout << "\nFile name '" << FName << "' succsefull reading. \n";


  // Считывание.

  int x = 0;//Переменные
  short int z = 0;
  float y = 0;
  char title[80];
  int inum = 0;

  /*НАЧАЛО РАБОТЫ С ФАЙЛОМ*/
  std::ifstream in(FName, std::ios::binary);

  in.read((char*)&title, sizeof(title));
  in.read((char*)&x, sizeof(x)); //перенос байтов из файла в "х"
  inum = x;
  std::cout << "number triangles " << inum << std::endl;
  //system("PAUSE");

  // освобождение оперативной памяти.


  //inumber_triangles_for_CAD_STL_model = inum;

  fig.root = new gCAD_STL;
  gCAD_STL* tmp = fig.root;

  // Смещение в м в глобальной системе координат каждого STL-файла.
  // позиционирование по месту.


  for (int i = 0; i < inum; ++i) {
    in.read((char*)&y, sizeof(y));  //перенос байтов из файла в "y"
    tmp->n.x =  y;
    in.read((char*)&y, sizeof(y));
    tmp->n.y =  y;
    in.read((char*)&y, sizeof(y));
    tmp->n.z =  y;

    in.read((char*)&y, sizeof(y));
    tmp->pa.x = scale_cad_import_data * y + fig.GLOBAL_MOVE_ALL_STL_IN_IMPORT_X;
    in.read((char*)&y, sizeof(y));
    tmp->pa.y = scale_cad_import_data * y + fig.GLOBAL_MOVE_ALL_STL_IN_IMPORT_Y;
    in.read((char*)&y, sizeof(y));
    tmp->pa.z = scale_cad_import_data * y + fig.GLOBAL_MOVE_ALL_STL_IN_IMPORT_Z;

    in.read((char*)&y, sizeof(y));
    tmp->pb.x = scale_cad_import_data * y + fig.GLOBAL_MOVE_ALL_STL_IN_IMPORT_X;
    in.read((char*)&y, sizeof(y));
    tmp->pb.y = scale_cad_import_data * y + fig.GLOBAL_MOVE_ALL_STL_IN_IMPORT_Y;
    in.read((char*)&y, sizeof(y));
    tmp->pb.z = scale_cad_import_data * y + fig.GLOBAL_MOVE_ALL_STL_IN_IMPORT_Z;

    in.read((char*)&y, sizeof(y));
    tmp->pc.x = scale_cad_import_data * y + fig.GLOBAL_MOVE_ALL_STL_IN_IMPORT_X;
    in.read((char*)&y, sizeof(y));
    tmp->pc.y = scale_cad_import_data * y + fig.GLOBAL_MOVE_ALL_STL_IN_IMPORT_Y;
    in.read((char*)&y, sizeof(y));
    tmp->pc.z = scale_cad_import_data * y + fig.GLOBAL_MOVE_ALL_STL_IN_IMPORT_Z;

    in.read((char*)&z, sizeof(z)); // это может быть информация о цвете.

    /*if (i < 2) {
      std::cout << "Test print to the console of the first two read triangles:" << std::endl;
      std::cout << "nx=" << tmp->n.x << " ny=" << tmp->n.y << " nz=" << tmp->n.z << std::endl;
      std::cout << "pa.x=" << tmp->pa.x << " pa.y=" << tmp->pa.y << " pa.z=" << tmp->pa.z << std::endl;
      std::cout << "pb.x=" << tmp->pb.x << " pb.y=" << tmp->pb.y << " pb.z=" << tmp->pb.z << std::endl;
      std::cout << "pc.x=" << tmp->pc.x << " pc.y=" << tmp->pc.y << " pc.z=" << tmp->pc.z << std::endl;
      //system("PAUSE");
    }*/


    if (i < inum - 1) {
      tmp->next = new gCAD_STL;
    }
    tmp = tmp->next;

  }

  in.close();
  /*КОНЕЦ РАБОТЫ С ФАЙЛОМ*/

}

// Функция для рисования куба
/*void drawCube() {
    glColor3f(1.0, 0.0, 0.0);
    glutSolidCube(1.0);
}*/

void DrawPrism(GLfloat xS, GLfloat yS, GLfloat zS, GLfloat xE, GLfloat yE, GLfloat zE)
{
    GLfloat V[8][3];  // Массив для хранения 8 вершин призмы

    // Установка координат вершин
    V[0][0] = xS; V[1][0] = xS; V[2][0] = xS; V[3][0] = xS;
    V[4][0] = xE; V[5][0] = xE; V[6][0] = xE; V[7][0] = xE;

    V[0][1] = yS; V[1][1] = yS; V[4][1] = yS; V[5][1] = yS;
    V[2][1] = yE; V[3][1] = yE; V[6][1] = yE; V[7][1] = yE;

    V[0][2] = zS; V[3][2] = zS; V[4][2] = zS; V[7][2] = zS;
    V[1][2] = zE; V[2][2] = zE; V[5][2] = zE; V[6][2] = zE;

    // Массив координат нормалей и индексов для построения граней
    GLfloat BoxNormals[6][3] = {
        {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
        {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
    };

    GLint BoxFaces[6][4] = {
        {4, 5, 6, 7}, {0, 1, 2, 3}, {2, 3, 7, 6},
        {0, 1, 5, 4}, {1, 2, 6, 5}, {0, 3, 7, 4}
    };

    for (GLint i = 0; i < 6; ++i) {
        glBegin(GL_QUADS);
            glNormal3fv(BoxNormals[i]);
            glVertex3fv(V[BoxFaces[i][0]]);
            glVertex3fv(V[BoxFaces[i][1]]);
            glVertex3fv(V[BoxFaces[i][2]]);
            glVertex3fv(V[BoxFaces[i][3]]);
        glEnd();
    }
}

// Функция для отрисовки тени
void drawShadow(RenderChessMan& figure,  int iaddx, int iaddy) {
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.06f); // смещение тени чуть ниже

    // Применяем матрицу проецирования для тени
  /*  GLfloat shadowMatrix[16] = {
        1, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };*/
    float a=A,b=B,c=C,z=fabs(zoom);
     GLfloat shadowMatrix[16] = {
        z+b, -a, 0, 0,
        -b, z+a, 0, 0,
        -b*c, -a*c, z, -c,
        0, 0, 0, z
    };
    /*GLfloat shadowMatrix[16] = {
       z+b, -a, 0, 0,
       -b, z+a, 0, -1,
       -b*c, -a*c, z, -c,
       0, 0, 0, z
   };*/
    /*GLfloat shadowMatrix[16] = {
      c, 0, a, a*c,
      0, c, b, b*c,
      0, 0, -z-c, -(c*z+b*c),
      0, 0, 1, 0
  };*/
    /*
    где  𝑎, 𝑏, и 𝑐
     — координаты источника света, а
    𝑍
     — глубина отображаемой сцены.

    Теперь поясним каждый элемент:

    𝑍
     — это глубина плоскости, на которую должно отображаться изображение тени.
    𝑎, 𝑏 и 𝑐
     — координаты источника света.
    */
    glMultMatrixf(shadowMatrix);

    //glColor3f(0.2f, 0.2f, 0.2f); // Цвет тени
    glColor3f(0.513f, 0.4414f, 0.3137f);// Цвет тени
    glScalef(1.0f, 1.0f, 0.2f);  // Сплющиваем по оси Z
    figure.draw(0.2f, 0.2f, 0.2f, iaddx, iaddy);

    glPopMatrix();
}

void drawFEN(std::string fen)
{
	// Рисуем доску.
	   for (int i_7=0; i_7<8; ++i_7) {
			 for (int j_7=0; j_7<8; ++j_7) {
				 if ((i_7+j_7)%2==0) {
					 glColor3f(0.5,0.5,0.5);// Сeрый
				 }
				 else {
					 glColor3f(0.5,0.78,1.0);// Голубой
				 }
				 DrawPrism(-4+i_7,-4+j_7,-0.05, -3+i_7,-3+j_7,0.05);
			 }
		 }

		 ChessPiece board[8][8];
		 // Изначально вся доска пустая.
 for (int i = 0; i < 8; i++)
 {
		 for (int j = 0; j < 8; j++)
		 {
				 board[j][i] = ChessPiece::EMPTY;
		 }
 }

 int i_b = 0;
 int i_w = 0;

int i_1 = 0; //
int i_2 = 9;
for (int i = 1; i <= 8; i++) {


 if (i_2 != 9) {

#ifndef DroidASS
		 std::cout << "ERROR ! incorrect FEN position.\n";
		 std::cout << "SECOND i_2=" << i_2 << " i_1=" << i_1 << std::endl;
		 std::cout << "FEN: " << fen << std::endl;

		 getchar();
#endif
		 break;
 }
 i_2 = 1;
 while (i_2 < 9)
 {
		 if (fen[i_1] == '1') {

				 i_2++;
				 i_1++;
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == '2') {

						 i_2 += 2;
						 i_1++;

				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == '3') {

						 i_2 += 3;
						 i_1++;

				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == '4') {

						 i_2 += 4;
						 i_1++;
				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == '5') {

						 i_2 += 5;
						 i_1++;

				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == '6') {

						 i_2 += 6;
						 i_1++;
				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == '7') {

						 i_2 += 7;
						 i_1++;
				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == '8') {

						 i_2 += 8;
						 i_1++;
				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == '/') {

						 if (i_2 != 1) {
#ifndef DroidASS
								 std::cout << "ERROR ! incorrect FEN position.\n";
								 std::cout << "FIRST i_2=" << i_2 << " i_1=" << i_1 << std::endl;
								 std::cout << "FEN: " << fen << std::endl;

								 getchar();
								 break;
#endif
						 }


						 //i_2:=i_2 + 8;
						 i_1++;
				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == 'r') {

						 board[i-1][i_2-1] = ChessPiece::B_ROOK;

						 i_b++;
						 i_2++;
						 i_1++;
				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == 'b') {

						 board[i-1][i_2-1] = ChessPiece::B_BISHOP;

						 i_b++;
						 i_2++;
						 i_1++;
				 }
		 }

		 if (i_2 < 9) {

				 if (fen[i_1] == 'p') {

						 board[i-1][i_2-1] = ChessPiece::B_PAWN;

						 i_b++;
						 i_2++;
						 i_1++;
				 }
		 }

		 if (i_2 < 9) {

				 if (fen[i_1] == 'n') {

						 board[i-1][i_2-1] = ChessPiece::B_KNIGHT;

						 i_b++;
						 i_2++;
						 i_1++;
				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == 'q') {

						 board[i-1][i_2-1] = ChessPiece::B_QUEEN;

						 i_b++;
						 i_2++;
						 i_1++;
				 }
		 }
		 if (i_2 < 9) {

				 if (fen[i_1] == 'k') {

						 board[i-1][i_2-1] = ChessPiece::B_KING;

						 i_b++;
						 i_2++;
						 i_1++;
				 }
		 }

		 if (i_2 < 9) {

				 if (fen[i_1] == 'R') {

						 board[i-1][i_2-1] = ChessPiece::W_ROOK;

						 i_w++;
						 i_2++;
						 i_1++;
				 }
		 }

		 if (i_2 < 9) {

				 if (fen[i_1] == 'B') {

						 board[i-1][i_2-1] = ChessPiece::W_BISHOP;

						 i_w++;
						 i_2++;
						 i_1++;
				 }
		 }

		 if (i_2 < 9) {

				 if (fen[i_1] == 'P') {

						 board[i-1][i_2-1] = ChessPiece::W_PAWN;

						 i_w++;
						 i_2++;
						 i_1++;
				 }
		 }

		 if (i_2 < 9) {

				 if (fen[i_1] == 'N') {

						 board[i-1][i_2-1] = ChessPiece::W_KNIGHT;

						 i_w++;
						 i_2++;
						 i_1++;
				 }
		 }

		 if (i_2 < 9) {

				 if (fen[i_1] == 'Q') {

						 board[i-1][i_2-1] = ChessPiece::W_QUEEN;

						 i_w++;
						 i_2++;
						 i_1++;
				 }
		 }

		 if (i_2 < 9) {

				 if (fen[i_1] == 'K') {

						 board[i-1][i_2-1] = ChessPiece::W_KING;

						 i_w++;
						 i_2++;
						 i_1++;
				 }
		 }

		 //std::cout << "i_1= " << i_1 << "fen[i_1]="<< fen[i_1] << " i_2= " << i_2 << std::endl;
		 //print();
		 //std::cout << fen << std::endl;
		 //getchar();
 }
}


while (fen[i_1] == ' ')
{
 i_1++;
}

	// white
float r0=0.4196f,g0=0.5547f,b0=0.1373f;
// black
float r1=1.0f,g1=0.753f,b1=0.796f;

for (int i_7=0; i_7<8; ++i_7) {
	for (int j_7=0; j_7<8; ++j_7) {
		switch(board[7-j_7][i_7]) {
	    case ChessPiece::W_BISHOP:
         bishop.draw(r0, g0, b0,i_7,j_7);
			break;
			case ChessPiece::B_BISHOP:
         bishop.draw(r1, g1, b1,i_7,j_7);
			break;
			case ChessPiece::W_ROOK:
         castle.draw(r0, g0, b0,i_7,j_7);
			break;
			case ChessPiece::B_ROOK:
         castle.draw(r1, g1, b1,i_7,j_7);
			break;
			case ChessPiece::W_PAWN:
				 pawn.draw(r0, g0, b0,i_7,j_7);
		 break;
		 case ChessPiece::B_PAWN:
				 pawn.draw(r1, g1, b1,i_7,j_7);
		 break;
		 case ChessPiece::W_QUEEN:
				queen.draw(r0, g0, b0,i_7,j_7);
		break;
		case ChessPiece::B_QUEEN:
				queen.draw(r1, g1, b1,i_7,j_7);
		break;
		case ChessPiece::W_KING:
			 king.draw(r0, g0, b0,i_7,j_7);
	 break;
	 case ChessPiece::B_KING:
			 king.draw(r1, g1, b1,i_7,j_7);
	 break;
	 case ChessPiece::W_KNIGHT:
			knight.draw(r0, g0, b0,i_7,j_7);
	break;
	case ChessPiece::B_KNIGHT:
			knight.draw(r1, g1, b1,i_7,j_7);
	break;
	case ChessPiece::EMPTY:
	break;
    }
	}
}

}

void drawChessBoard() {

// Рисуем доску.
   for (int i_7=0; i_7<8; ++i_7) {
		 for (int j_7=0; j_7<8; ++j_7) {
			 if ((i_7+j_7)%2==0) {
				 glColor3f(0.5,0.5,0.5);// Сeрый
			 }
			 else {
				 glColor3f(0.5,0.78,1.0);// Голубой
			 }
			 DrawPrism(-4+i_7,-4+j_7,-0.05, -3+i_7,-3+j_7,0.05);
		 }
	 }



	 // Отрисовка тени фигур
	    drawShadow(bishop,  2, 0);
	    drawShadow(bishop,  5, 0);
	    drawShadow(castle,  0, 0);
	    drawShadow(castle,  7, 0);
	    drawShadow(king,  4, 0);
	    drawShadow(pawn,  0, 1);
	    drawShadow(pawn,  1, 1);
	    drawShadow(pawn,  2, 1);
	    drawShadow(pawn,  3, 1);
	    drawShadow(pawn,  4, 1);
	    drawShadow(pawn,  5, 1);
	    drawShadow(pawn,  6, 1);
	    drawShadow(pawn,  7, 1);
	    drawShadow(queen,  3, 0);
	    drawShadow(knight, 1, 0);
	    drawShadow(knight, 6, 0);
	    drawShadow(pawn,  0, 6);
	    drawShadow(pawn,  1, 6);
	    drawShadow(pawn,  2, 6);
	    drawShadow(pawn,  3, 6);
	    drawShadow(pawn,  4, 6);
	    drawShadow(pawn,  5, 6);
	    drawShadow(pawn,  6, 6);
	    drawShadow(pawn,  7, 6);
	    drawShadow(bishop, 2, 7);
	    drawShadow(bishop, 5, 7);
	    drawShadow(castle, 0, 7);
	    drawShadow(castle, 7, 7);
	    drawShadow(king,  4, 7);
	    drawShadow(queen,  3, 7);
	    drawShadow(knight,  1, 7);
	    drawShadow(knight,  6, 7);

			// white
			float r0=0.4196f,g0=0.5547f,b0=0.1373f;

   bishop.draw(r0, g0, b0,2,0);
	 bishop.draw(r0, g0, b0,5,0);
   castle.draw(r0, g0, b0,0,0);
	 castle.draw(r0, g0, b0,7,0);
   king.draw(r0, g0, b0,4,0);
   pawn.draw(r0, g0, b0,0,1);
	 pawn.draw(r0, g0, b0,1,1);
	 pawn.draw(r0, g0, b0,2,1);
	 pawn.draw(r0, g0, b0,3,1);
	 pawn.draw(r0, g0, b0,4,1);
	 pawn.draw(r0, g0, b0,5,1);
	 pawn.draw(r0, g0, b0,6,1);
	 pawn.draw(r0, g0, b0,7,1);
   queen.draw(r0, g0, b0,3,0);
   knight.draw(r0, g0, b0,1,0);
	 knight.draw(r0, g0, b0,6,0);

   // black
	float r1=1.0f,g1=0.753f,b1=0.796f;
   pawn.draw(r1, g1, b1,0,6);
	 pawn.draw(r1, g1, b1,1,6);
	 pawn.draw(r1, g1, b1,2,6);
	 pawn.draw(r1, g1, b1,3,6);
	 pawn.draw(r1, g1, b1,4,6);
	 pawn.draw(r1, g1, b1,5,6);
	 pawn.draw(r1, g1, b1,6,6);
	 pawn.draw(r1, g1, b1,7,6);
	 bishop.draw(r1, g1, b1,2,7);
	 bishop.draw(r1, g1, b1,5,7);
   castle.draw(r1, g1, b1,0,7);
	 castle.draw(r1, g1, b1,7,7);
   king.draw(r1, g1, b1,4,7);
	 queen.draw(r1, g1, b1,3,7);
   knight.draw(r1, g1, b1,1,7);
	 knight.draw(r1, g1, b1,6,7);
}

// Настройка освещения
void setupLighting() {
    GLfloat light0_position[] = { A, B, C, 0.0 };
    GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

// Включение режима цветного материала
void enableColorMaterial() {
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    // Опционально можно указать также GL_SPECULAR, если нужно
}

// Функция, вызываемая для рендеринга сцены
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, zoom); // Используем переменную для зума
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
		glRotatef(angleZ, 0.0, 0.0, 1.0);

  /*
  // Настройка материала
     GLfloat mat_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
     GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
     GLfloat mat_shininess[] = { 50.0 };

     glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
     glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
     glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  */

    //drawCube();
    //drawChessBoard();
		//drawFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		// Duchess-Kaissa 2-й компьютерный
		// шахматный чемптонат Торонто, 1977
		// Ход белых Фa8. Каисса сыграла Лe8.
		drawFEN("6k1/4rp1p/Qp1q1bp1/1B1n4/3Pp1P1/4B2P/PP3P2/2R3K1 w - - 0 1");
    glutSwapBuffers();
}

// Функция для обработки изменения размеров окна
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// Функция для обработки событий перемещения мыши
void mouseMotion(int x, int y) {
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;

    angleX += deltaY * 0.5f;
    angleY += deltaX * 0.5f;

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}

// Функция для обработки событий нажатия кнопок мыши
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastMouseX = x;
        lastMouseY = y;
    }
}

// Функция для обработки прокрутки колесика мыши
/*void mouseWheel(int wheel, int direction, int x, int y) {
    if (direction > 0) {
        zoom += 0.5f; // Приближение
    } else {
        zoom -= 0.5f; // Удаление
    }

    glutPostRedisplay();
}*/

// Функция для обработки событий нажатия клавиш
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            zoom -= 0.5f;
            break;
        case GLUT_KEY_DOWN:
            zoom += 0.5f;
            break;
				case GLUT_KEY_LEFT:
		            angleZ -= 5.0f;
		            break;
		    case GLUT_KEY_RIGHT:
		            angleZ += 5.0f;
		            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {

    // Загрузка геометрии.
    ReadSTL_binary(bishop);
    ReadSTL_binary(castle);
    ReadSTL_binary(king);
    ReadSTL_binary(knight);
    ReadSTL_binary(pawn);
    ReadSTL_binary(queen);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("GLUT Mouse Rotation");

    glEnable(GL_DEPTH_TEST);

    // Настраиваем освещение
    setupLighting();

    // Включаем color material
    enableColorMaterial();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseButton);
    //glutMouseWheelFunc(mouseWheel); // Устанавливаем функцию для обработки прокрутки колесика
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
