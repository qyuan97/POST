//
// Created by qyxie on 16/1/2022.
//
#include<cryptopp/rsa.h>
#include<cryptopp/osrng.h>
#include<cryptopp/hex.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha3.h>
#include <cryptopp/files.h>
#include<iostream>
#include<vector>

std::string sha3_256(std::string msg){
    std::string digest;
    CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(std::cout));

    CryptoPP::SHA3_256 hash;
    hash.Update((const byte*)msg.data(), msg.size());
    digest.resize(hash.DigestSize());
    hash.Final((byte*)&digest[0]);

    CryptoPP::StringSource(digest, true, new CryptoPP::Redirector(encoder));
    return digest;
}

std::string hmac_sha3(std::string key, std::string msg){
    std::string mac, encoded;

    try
    {
        CryptoPP::HMAC<CryptoPP::SHA3_256> hmac(key, key.size());

        CryptoPP::StringSource ss2(msg, true,
                         new CryptoPP::HashFilter(hmac,
                                        new CryptoPP::StringSink(mac)
                         ) // HashFilter
        ); // StringSource
    }
    catch(const CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    encoded.clear();
    CryptoPP::StringSource ss3(mac, true,
                     new CryptoPP::HexEncoder(
                             new CryptoPP::StringSink(encoded)
                     ) // HexEncoder
    ); // StringSource
    return encoded;
}

std::pair<CryptoPP::Integer, CryptoPP::Integer> setup(int n_bits){
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, n_bits);

    const CryptoPP::Integer& p = params.GetPrime1();
    const CryptoPP::Integer& q = params.GetPrime2();

    return std::make_pair(p, q);
}

void PoR_setup(std::string PoR_sk[], int n){
    for(int i = 0; i < n; i++){
        CryptoPP::AutoSeededRandomPool prng;
        CryptoPP::SecByteBlock key(16);
        prng.GenerateBlock(key, key.size());

        std::string s;
        CryptoPP::HexEncoder hex(new CryptoPP::StringSink(s));
        hex.Put(key, key.size());
        hex.MessageEnd();

        PoR_sk[i] = s;
    }
}

// TODO Store
void store(std::string Por_sk[]){

    std::vector<std::string> challenge_set;
    std::vector<std::string> verify_set;
    challenge_set.clear();
    verify_set.clear();


}

// TODO Verification
void prove(){

}

int main (int argc, char* argv[])
{
    const int n_bits = 1024;
    const int PoR_n = 32;

    std::string PoR_sk[PoR_n] = {""};
    PoR_setup(PoR_sk, PoR_n);

//    std::cout << "PoR_sk[0]: " << PoR_sk[0] << std::endl;

    auto pq = setup(n_bits);
    const CryptoPP::Integer p = pq.first;
    const CryptoPP::Integer q = pq.second;

    std::cout << "p: " << p << std::endl;
    std::cout << "q: " << q << std::endl;

//    std::string msg = "Yoda said, Do or do not. There is no try.";
//    std::string dig = sha3_256(msg);
//    std::cout << dig << std::endl;

    std::string msg = "Yoda said, Do or do not. There is no try.";
    std::string cipher = hmac_sha3(PoR_sk[0], msg);
    std::cout << cipher << std::endl;

    return 0;
}