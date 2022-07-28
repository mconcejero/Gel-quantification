#include "proceso.h"
#include "ui_mainwindow.h"

Mat img, imggray, imgnew, output;

vector<Point> pozillos;
int npoz=0;
int th=120;
int cols=0;
int rows=0;
int sel=0;
bool selec=false;
int zoom=100;

vector<vector<Point>> contours;
vector<vector<Point>> contPoly ;
vector<Rect> boundRect;
vector<int> elim;
vector<long> qband;

ofstream ficherocsv;

void locator(int event, int x, int y, int flags, void* userdata);
void ordenar_bandas();
vector<long> cuantifica_bandas();


bool comienza(String geles)
{
  pozillos.clear();
  contours.clear();
  contPoly.clear();

  namedWindow("Modificadores");
  cout << geles << endl;
      img = imread(geles);
  namedWindow("Threshold");
  if (!img.data)
  {
      cout << "Error cargando la imagen" << endl;
      return false;
  }
  Mat thres;
  cvtColor(img, imggray,COLOR_BGR2GRAY); // Pasamos la imagen a tonos grises
  bitwise_not(imggray,imggray); // Y la invertimos
  threshold(imggray,thres,10,255,THRESH_BINARY); //La pasamos a binario para detectar
                                                  // los bordes del gel.
  //vector<vector<Point>> areas;
  findContours(thres,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE); // Busca los contornos
  int cont=0;
  float area=0;
  Rect gel;
  // Buscamos el mayor área, que se debe corresponder con el gel, para cortar la imagen.
  for (int i=0; i<contours.size(); i++)
  {
      float are = contourArea(contours[i],false);
      if (are > area)
      {
          area=are;
          cont=i;
      }
  }
  if (cont>0)
  {
      drawContours(img,contours,cont,Scalar(0,255,0),1);
      gel=boundingRect(contours[cont]);
      imgnew=imggray(gel);
      imshow("Newgel",imgnew);
  }
  else
  {
      destroyAllWindows();
      return false;
  }
  //imshow("Gel",img);
  createTrackbar("Threshold","Modificadores",&th,255);
  createTrackbar("Zoom","Modificadores",&zoom,100);
  contours.clear();                                          // los bordes del gel.
  bitwise_not(imgnew,imgnew);

  Mat imgnew2;
  //imshow("Newgel2",imgnew2);
  namedWindow("Bandas");
  setMouseCallback("Bandas",locator,NULL);
  while (true)
  {
      resize(imgnew,imgnew2,Size(),(zoom/100.00),(zoom/100.00));
      contPoly.clear();
      output = Mat::zeros(imgnew2.rows,imgnew2.cols,CV_8UC3);
      //medianBlur(imgnew2,imgnew2,5);
      threshold(imgnew2,thres,th,255,THRESH_BINARY); //La pasamos a binario para detectar

      findContours(thres,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE); // Busca los contornos
      contPoly.resize(contours.size());
      boundRect.clear();
      for (int i=0; i<contours.size(); i++)
      {
          float peri=arcLength(contours[i],true);
          approxPolyDP(contours[i],contPoly[i],peri*0.02,true);
          boundRect.push_back(boundingRect(contPoly[i]));
          auto color=Scalar(0,255,0);
          if ((sel==i) and (selec==true))
          {
              color=Scalar(255,0,0);
          }
          bool yel=false;
          for(int j=0;j<elim.size();j++)
          {
              if (elim[j]==i)
                  yel=true;
          }
          if (yel==false)
          {
              drawMarker(output,Point(boundRect[i].x+boundRect[i].width/2,boundRect[i].y+boundRect[i].height/2),color,2);
              rectangle(output,boundRect[i],color,2);
              drawContours(output,contours,i,color,1);
          }
      }
      imshow("Newgel",imgnew2);
      imshow("Threshold",thres);
      imshow("Bandas",output);
      char key=waitKey(30);
      if (key=='c')
      {
          cout << "Comenzamos a ordenar las bandas y integrarlas. " << endl;
          qband.clear();
          ordenar_bandas();
          qband=cuantifica_bandas();
          destroyAllWindows();
          cout << "Guardando los datos de cuantificacion" << endl;
          //gdatos();
          break;
      }
      else if (key=='q')
      {
          destroyAllWindows();
          return false;
      }
  }
  return true;

}

void guarda(String fichero)
{
  ficherocsv.open(fichero);
  ficherocsv << "X;Y;Area";
  for(int i=0; i< boundRect.size(); i++)
  {
      String cadena = "\n" + to_string(boundRect[i].x + boundRect[i].width/2) + ";" + to_string(boundRect[i].y + boundRect[i].height/2);
      cadena += ";" + to_string(qband[i]) ;
      ficherocsv << cadena;
  }
  ficherocsv.close();

}

void ordenar_bandas()
{
    cout << " De " << boundRect.size() << " bandas, nos quedamos con ";
    for (int i=0; i<contours.size(); i++)
    {
        bool eli=false;
        for (int j=0; j<elim.size(); j++)
        {
            if (elim[j]==i)
            {
                boundRect.erase(boundRect.begin() + i);
                elim.erase(elim.begin()+j);
                i--;
                break;
            }
        }
    }
    cout << boundRect.size() << " bandas para integrar" << endl;
}

vector<long> cuantifica_bandas()
{
    vector<long> quant;
    for (int i=0; i<boundRect.size(); i++)
    {
        Mat imgq = imgnew(boundRect[i]);
        long b=0;
        for(int c=0; c<imgq.cols; c++)
        {
            for (int r=0; r<imgq.rows; r++)
            {
                b= b + imgq.at<uchar>(r,c);
            }
        }
        quant.push_back(b);
    }
    return quant;
}

void locator(int event, int x, int y, int flags, void* userdata)
{

    for (int i=0; i<boundRect.size(); i++)
    {
        //drawMarker(output,centers[i],Scalar(0,0,255),2);
        //drawContours(output,contours,i,Scalar(0,255,0),1);
        if ((x>boundRect[i].x) and (y>boundRect[i].y))
        {
            if ((x<(boundRect[i].x+boundRect[i].width)) and (y<(boundRect[i].y+boundRect[i].height)))
            {
                bool yel=false;
                for (int j=0; j<elim.size(); j++)
                {
                    if (elim[j]==i)
                    {
                        yel=true;
                    }
                }
                if (yel==false)
                {
                    sel=i;
                    selec=true;
                    break;
                }
            }
            else
            {
                selec=false;
            }
        }
        else
            selec=false;
    }
    if (event==EVENT_LBUTTONDOWN)
    {
       if (selec==true)
            elim.push_back(sel);
    }
    else if(event==EVENT_RBUTTONDOWN)
    {
        elim.clear();
    }

    imshow("Bandas",output);
}
