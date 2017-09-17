#include <math.h>

static inline void flycamera(
    float * cam_vec3, float * cam_quat, float * cam_m3x3,
    float look_mult, float move_mult,
    float x_delta, float y_delta,
    float key_f, float key_l, float key_b, float key_r, float key_u, float key_d)
{
  // y_quat = quatFromAxisAngle({1, 0, 0}, y_delta * look_mult * to_radians)
  // x_quat = quatFromAxisAngle({0, 1, 0}, x_delta * look_mult * to_radians)

  float ydt = y_delta * look_mult * 0.017453f;
  float xdt = x_delta * look_mult * 0.017453f;
  float yqx = (float)sin(ydt);
  float yqw = (float)cos(ydt);
  float xqy = (float)sin(xdt);
  float xqw = (float)cos(xdt);

  // cam_quat = cam_quat * y_quat
  // cam_quat = x_quat * cam_quat

  float iqx = cam_quat[0];
  float iqy = cam_quat[1];
  float iqz = cam_quat[2];
  float iqw = cam_quat[3];

  float tqx = +iqw * yqx + iqx * yqw;
  float tqy = -iqz * yqx - iqy * yqw;
  float tqz = -iqy * yqx + iqz * yqw;
  float tqw = +iqx * yqx - iqw * yqw;

  float cqx = +tqz * xqy + tqx * xqw;
  float cqy = -tqw * xqy - tqy * xqw;
  float cqz = -tqx * xqy + tqz * xqw;
  float cqw = +tqy * xqy - tqw * xqw;

  // x_axis = rotateVec3ByQuat({-1, 0, 0}, cam_quat)
  // y_axis = rotateVec3ByQuat({ 0, 1, 0}, cam_quat)
  // z_axis = rotateVec3ByQuat({ 0, 0,-1}, cam_quat)

  float qxqx = cqx * cqx;
  float qxqy = cqx * cqy;
  float qxqz = cqx * cqz;
  float qxqw = cqx * cqw;

  float qyqy = cqy * cqy;
  float qyqz = cqy * cqz;
  float qyqw = cqy * cqw;

  float qzqz = cqz * cqz;
  float qzqw = cqz * cqw;

  float xax = (qyqy + qzqz) *  2.f - 1.f;
  float xay = (qxqy + qzqw) * -2.f;
  float xaz = (qyqw - qxqz) *  2.f;

  float yax = (qxqy - qzqw) *  2.f;
  float yay = (qxqx + qzqz) * -2.f + 1.f;
  float yaz = (qxqw + qyqz) *  2.f;

  float zax = (qyqw + qxqz) * -2.f;
  float zay = (qxqw - qyqz) *  2.f;
  float zaz = (qxqx + qyqy) *  2.f - 1.f;

  float xm = key_l - key_r;
  float ym = key_u - key_d;
  float zm = key_f - key_b;

  float mx = xax * xm + yax * ym + zax * zm;
  float my = xay * xm + yay * ym + zay * zm;
  float mz = xaz * xm + yaz * ym + zaz * zm;

  float ln = (float)sqrt(mx * mx + my * my + mz * mz);
  if (ln != 0) {
    mx /= ln;
    my /= ln;
    mz /= ln;
  }

  cam_vec3[0] += mx * move_mult;
  cam_vec3[1] += my * move_mult;
  cam_vec3[2] += mz * move_mult;

  cam_quat[0] =  cqx;
  cam_quat[1] =  cqy;
  cam_quat[2] =  cqz;
  cam_quat[3] =  cqw;

  cam_m3x3[0] = -xax;
  cam_m3x3[1] =  yax;
  cam_m3x3[2] = -zax;

  cam_m3x3[3] = -xay;
  cam_m3x3[4] =  yay;
  cam_m3x3[5] = -zay;

  cam_m3x3[6] = -xaz;
  cam_m3x3[7] =  yaz;
  cam_m3x3[8] = -zaz;
}
