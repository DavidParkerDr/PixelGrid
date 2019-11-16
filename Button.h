class Button {
  private: 
  uint16_t mPin;
  uint16_t mPreviousState;
  bool mReleased;
  bool mPressed;

  public:
  Button(uint16_t pPin ){
    mPin = pPin;  
    mPreviousState = LOW;
    mReleased = false;
    mPressed = false;
  }

  bool isDown() {
    uint16_t buttonState = digitalRead(mPin);
    if(buttonState == LOW) {
      return true;
    }
    return false;
  }
  bool isUp() {
    uint16_t buttonState = digitalRead(mPin);
    if(buttonState == LOW) {
      return true;
    }
    return false;
  }
  bool released() {    
    return mReleased;
  }
bool pressed() {    
    return mPressed;
  }
  void update() {
    uint16_t buttonState = digitalRead(mPin);
    if(buttonState == LOW) {
      if(mPreviousState == HIGH) {
        // button released
        mReleased = true;
      }
      else {
        mReleased = false;
      }
    }
    else {
      if(mPreviousState == LOW) {
        // button pressed
        mPressed = true;
      }
      else {
        mPressed = false;
      }
    }
    mPreviousState = buttonState;
  }
};

