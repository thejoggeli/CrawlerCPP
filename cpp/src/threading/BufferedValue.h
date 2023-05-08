#pragma once

namespace Crawler {

template<typename T>
class BufferedValue {
private:

public:

    T value;
    T bufferedValue;
    bool needsUpdate = false;

    BufferedValue(){
        value = 0;
        bufferedValue = 0;
    }

    BufferedValue(const BufferedValue& b){
        this->value = b.value;
        this->bufferedValue = b.bufferedValue;
    }

    inline operator T() const { 
        return this->value;
    }

    inline BufferedValue& operator = (T value){
        this->value = value;
        needsUpdate = false;
        return *this;
    }

    inline BufferedValue& SetValue(T value){
        this->value = value;
        needsUpdate = false;
        return *this;
    }

    inline void BufferValue(T value){
        this->bufferedValue = value;
        needsUpdate = this->bufferedValue != this->value;
    }

    inline void ApplyBuffer(){
        if(needsUpdate){
            this->value = bufferedValue;
        }
        needsUpdate = false;
    }

};

}

