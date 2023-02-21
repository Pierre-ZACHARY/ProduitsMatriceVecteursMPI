//
// Created by Pierre Zachary on 21/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_MESSAGEPAYLOAD_HPP
#define PRODUITSMATRICEVECTEURSMPI_MESSAGEPAYLOAD_HPP

class MessagePayload {
public:
    unsigned char* data; // bytes
    size_t size;
    explicit MessagePayload(size_t size) : size(size){
        data = new unsigned char[size];
    }
    MessagePayload(unsigned char* pdata, size_t size) : size(size) {
        this->data = pdata;
    }
};


#endif //PRODUITSMATRICEVECTEURSMPI_MESSAGEPAYLOAD_HPP
