#include <FixedPoints.h>
#include <FixedPointsCommon.h>

extern "C"{
#include <avrfix.h>
}

#include <Arduboy.h>

Arduboy arduboy;

typedef SFixed<11,5> intermediate_t;
typedef SFixed<4,4> point_t;

const int8_t fl = 75;
const int8_t screen_x = 64;
const int8_t screen_y = 32;
intermediate_t tx_ = 0;
intermediate_t ty_ = 0;
intermediate_t tz_ = 5;
const float delta_rad_ = PI/120;
float rad_x_ = 0;
float rad_y_ = 0;
float rad_z_ = 0;

intermediate_t transform_ [12];

const point_t points_[] = { -1,-1,-1,
                          -1,1,-1,
                          1,1,-1,
                          1,-1,-1,
                          -1,-1,1,
                          -1,1,1,
                          1,1,1,
                          1,-1,1};
                                
const int8_t edges_[]  = {0,1,
                          1,2,
                          2,3,
                          3,0,
                          4,5,
                          5,6,
                          6,7,
                          7,4,
                          0,4,
                          1,5,
                          2,6,
                          3,7};

int8_t num_pts_ = 8;
uint8_t num_edges_ = 12;
                                
int8_t p2d_[16];

intermediate_t fast_sin(float angle){
  return intermediate_t(lktod(lsinlk(ftolk(angle))));
}

intermediate_t fast_cos(float angle){
  return intermediate_t(lktod(lcoslk(ftolk(angle))));
}

void build_transform(float xr, float yr, float zr, intermediate_t xo, intermediate_t yo, intermediate_t zo, intermediate_t *transform){
  intermediate_t b = fast_sin(xr); //b
  intermediate_t d = fast_sin(yr); //d
  intermediate_t f = fast_sin(zr); //f
  intermediate_t a = fast_cos(xr); //a
  intermediate_t c = fast_cos(yr); //c
  intermediate_t e = fast_cos(zr); //e

  transform[0] = (c*e);
  transform[1] = (-c*f);
  transform[2] = (d);
  transform[3] = (b*d*e+a*f);
  transform[4] = (a*e-b*d*f);
  transform[5] = (-b*c);
  transform[6] = (b*f-a*d*e);
  transform[7] = (b*e+a*d*f);
  transform[8] = (a*c);
  transform[9] = (xo);
  transform[10] = (yo);
  transform[11] = (zo);
}

void transform_point(point_t *ptin, intermediate_t *ptout, intermediate_t *transform){
  intermediate_t pt_16[] = {(intermediate_t)ptin[0],(intermediate_t)ptin[1],(intermediate_t)ptin[2]};
  ptout[0] = pt_16[0] * transform[0] + pt_16[1] * transform[3] + pt_16[2] * transform[6] + transform[9];
  ptout[1] = pt_16[0] * transform[1] + pt_16[1] * transform[4] + pt_16[2] * transform[7] + transform[10];
  ptout[2] = pt_16[0] * transform[2] + pt_16[1] * transform[5] + pt_16[2] * transform[8] + transform[11];
}

void project_point(intermediate_t *pt3d, int8_t* pt2d){
  intermediate_t factor = fl / pt3d[2];
  pt2d[0] = (int8_t)((int16_t)((factor * pt3d[0]))) + screen_x;
  pt2d[1] = (int8_t)((int16_t)((factor * pt3d[1]))) + screen_y;
}

void draw_3d( point_t *points_3d, int8_t *points_2d, intermediate_t *transform, int8_t num_points, int8_t *edges, int8_t num_edges){
  intermediate_t transformed[3];
  for (int i = 0; i < num_points; i++){
    transform_point(points_3d+i*3,transformed,transform);
    project_point(transformed, points_2d+i*2);
  }
  for (int i = 0;i < num_edges; i++){
    arduboy.drawLine(points_2d[edges[i*2]*2],points_2d[edges[i*2]*2+1],points_2d[edges[i*2+1]*2],points_2d[edges[i*2+1]*2+1],255);
  }
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
}

void loop() {
    if (!(arduboy.nextFrame()))
    return;
   arduboy.clear();
   arduboy.setCursor(0,0);

   if (arduboy.pressed(UP_BUTTON)){
      rad_x_ += delta_rad_;
      if (rad_x_ >= 2*PI) rad_x_ -= 2*PI;
   }else if (arduboy.pressed(DOWN_BUTTON)){
      rad_x_ -= delta_rad_;
      if (rad_x_ < 0) rad_x_ += 2*PI;
   }

   if (arduboy.pressed(RIGHT_BUTTON)){
      rad_y_ += delta_rad_;
      if (rad_y_ >= 2*PI) rad_y_ -= 2*PI;
   }else if (arduboy.pressed(LEFT_BUTTON)){
      rad_y_ -= delta_rad_;
      if (rad_y_ < 0) rad_y_ += 2*PI;
   }

   if (arduboy.pressed(A_BUTTON)){
      rad_z_ += delta_rad_;
      if (rad_z_ >= 2*PI) rad_z_ -= 2*PI;
   }else if (arduboy.pressed(B_BUTTON)){
      rad_z_ -= delta_rad_;
      if (rad_z_ < 0) rad_z_ += 2*PI;
   }
   build_transform(rad_x_,rad_y_,rad_z_,tx_,ty_,tz_,transform_);
   draw_3d(points_,p2d_,transform_,num_pts_,edges_,num_edges_);
   arduboy.display();
}
