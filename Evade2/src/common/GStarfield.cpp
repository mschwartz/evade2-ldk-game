#include "GStarfield.h"
#include "GCamera.h"

// Based off of https://github.com/ankorite/Arduboy-Starfield-Demo/blob/master/src/starfield/starfield.ino
#ifdef OLD_STARFIELD
GStarfield::GStarfield() : BPlayfield() {
  for (TInt i = 0; i < NUM_STARS; i++) {
    InitStar(i);
  }
}

GStarfield::~GStarfield() noexcept {

}

void GStarfield::Render() {
  gDisplay.renderBitmap->Clear(0);
  TFloat cz = gCamera->z,
         sw = TFloat(SCREEN_WIDTH),
         sh = TFloat(SCREEN_HEIGHT);

  for (int i = 0; i < NUM_STARS; i++) {
    TFloat zz = (mStarZ[i] - cz) * 2;
    if (zz < 0) {
      InitStar(i);
      zz = (mStarZ[i] - cz) * 2;
    }
    TFloat ratioX = sw / (zz + sw);
    TFloat ratioY = sh / (zz + sh);
    TFloat x      = (sw / 2) - (mStarX[i] - gCamera->x) * ratioX;
    TFloat y      = (sh / 2) - (mStarY[i] - gCamera->y) * ratioY;

    if (x < 0) {
//      printf("InitStar x %f < 0\n", x);
      InitStar(i);
    } else if (x > sw) {
//      printf("InitStar x %f > %f\n", x, sw);
      InitStar(i);
    } else if (y < 0) {
//      printf("InitStar y %f < 0\n", y);
      InitStar(i);
    } else if (y > sh) {
//      printf("InitStar y %f > %f\n", y, sh);
      InitStar(i);
    }
//    printf("Plot %f,%f\n", x, y);
    gDisplay.renderBitmap->SafeWritePixel(x, y, COLOR_STAR);
  }
}

void GStarfield::InitStar(TInt aIndex) {
  mStarX[aIndex] = TFloat(256) - Random(0, 512) + gCamera->x;
  mStarY[aIndex] = TFloat(256) - Random(0, 512) + gCamera->y;
  mStarZ[aIndex] = gCamera->z + Random(200, 512);
}

#else

GStarfield::GStarfield() : BPlayfield(){
  mCurrSpeed = 10;
  mMinSpeed = 1;
  mBoostSpeed = EFalse;
  mWarp = EFalse;

  for (TInt i = 0; i < NUM_STARS; i++) {
    InitStar(i, 0);
  }
}

void GStarfield::Render() {
  gDisplay.renderBitmap->Clear(0);


  TFloat travelX = 0,
      travelY = 0,
      travelZ = 0;

  // Todo: this needs to be in Player.cpp or GGame.cpp
  TBool jsRight = gControls.IsPressed(JOYRIGHT),
      jsLeft = gControls.IsPressed(JOYLEFT),
      jsUp = gControls.IsPressed(JOYUP),
      jsDown = gControls.IsPressed(JOYDOWN),
      jsRButton = gControls.IsPressed(BUTTONR),
      jsLButton = gControls.IsPressed(BUTTONL);


  //Todo: ROTATE via L or R Buttons in Z dimension
  if (gGame->GetState() == GAME_STATE_GAME) {
    // rotate
    if (jsUp) {
      travelY = .015;
    }

    if (jsDown) {
      travelY = -.015;
    }

    if (jsLeft) {
      travelZ = -.015;
    }

    if (jsRight) {
      travelZ = .015;
    }

//    if (jsLButton) {
//      printf("TODO: Rotate stars & play field on L button\n");
//      travelX = .02;
//    }
//    if (jsRButton) {
//      printf("TODO: Rotate stars & play field on R button\n");
//      travelX = -.02;
//    }

  }
  if (gCamera->vz == CAMERA_WARP_VZ) {
    mBoostSpeed = ETrue;
  }
  else {
    mBoostSpeed = EFalse;
  }

  // Loop through each star.
  for (TInt16 i = 0; i < NUM_STARS; i++) {
    stars[i].mZ -= mCurrSpeed;

    if (travelY != 0) {
      TFloat temp_y = stars[i].mY;
      TFloat temp_z = stars[i].mZ;
      stars[i].mY = temp_y * cos(travelY) - temp_z * sin(travelY);
      stars[i].mZ = temp_z * cos(travelY) + temp_y * sin(travelY);
    }

    if (travelX != 0) {
      TFloat temp_x = stars[i].mX;
      TFloat temp_y = stars[i].mY;
      stars[i].mX = temp_x * cos(travelX) - temp_y * sin(travelX);
      stars[i].mY = temp_y * cos(travelX) + temp_x * sin(travelX);
    }

    if (travelZ != 0) {
      TFloat temp_x = stars[i].mX;
      TFloat temp_z = stars[i].mZ;
      stars[i].mX = temp_x * cos(travelZ) - temp_z * sin(travelZ);
      stars[i].mZ = temp_z * cos(travelZ) + temp_x * sin(travelZ);
    }

//    if (travelX != 0) {
//      TFloat temp_x = stars[i].mX;
//      TFloat temp_y = stars[i].mY;
//      stars[i].mX = temp_x * cos(travelX) - temp_y * sin(travelX);
//      stars[i].mY = temp_y * cos(travelX) + temp_x * sin(travelX);
//    }
//
//    if (travelZ != 0) {
//      TFloat temp_x = stars[i].mX;
//      TFloat temp_z = stars[i].mZ;
//      stars[i].mX = temp_x * cos(travelZ) - temp_z * sin(travelZ);
//      stars[i].mZ = temp_z * cos(travelZ) + temp_x * sin(travelZ);
//    }



    stars[i].mScreenX = stars[i].mX / stars[i].mZ * 100 + SCREEN_WIDTH / 2;
    stars[i].mScreenY = stars[i].mY / stars[i].mZ * 100 + SCREEN_HEIGHT / 2;

    actualTime = Milliseconds() * .05;
    if (mBoostSpeed && mCurrSpeed <= STAR_SPEED_MAX && actualTime - speedMills >= 25) {
      mCurrSpeed = mCurrSpeed + 2.5;
      speedMills = actualTime;
    }

    if (!mBoostSpeed && mCurrSpeed > STAR_SPEED_MIN && actualTime - speedMills >= 10) {
      mCurrSpeed -= 2.5;
      speedMills = actualTime;
    }
    if (mCurrSpeed < STAR_SPEED_MIN) {
      mCurrSpeed = STAR_SPEED_MIN;
    }


    //If the stars go off the screen remove them and re-draw. If the stars hang out in the center remove them also
    if (stars[i].mScreenX > SCREEN_WIDTH || stars[i].mScreenX < 0 || stars[i].mScreenY > SCREEN_HEIGHT ||
        stars[i].mScreenY < 0 ||
        (stars[i].mScreenX == SCREEN_WIDTH >> 1 && stars[i].mScreenY == SCREEN_HEIGHT >> 1)) {


      int xMin = -1000,
          xMax = 1000,
          yMin = -500,
          yMax = 500;

      if ((gGame->GetState() == GAME_STATE_GAME) && (jsLeft || jsRight)) {
        xMin = -1000;
        xMax = 1000;
      }

      stars[i].Randomize(
        xMin,
        xMax,
        yMin,
        yMax,
        RANDOM_Z_MIN,
        RANDOM_Z_MAX,
        STAR_SPEED_MIN,
        STAR_SPEED_MAX
      );

      stars[i].mScreenX = stars[i].mX / stars[i].mZ * 100 + SCREEN_WIDTH / 2;
      stars[i].mScreenY = stars[i].mY / stars[i].mZ * 100 + SCREEN_HEIGHT / 2;
      stars[i].mOldScreenX = stars[i].mScreenX;
      stars[i].mOldScreenY = stars[i].mScreenY;
    }


    // Draw the star at its new coordinate.
    gDisplay.renderBitmap->DrawLine(
        ENull,
        stars[i].mScreenX,
        stars[i].mScreenY,
        stars[i].mOldScreenX,
        stars[i].mOldScreenY,
        STAR_COLOR
    );

    //keep track of the old spot
    stars[i].mOldScreenX = stars[i].mScreenX;
    stars[i].mOldScreenY = stars[i].mScreenY;
  }

}

#endif