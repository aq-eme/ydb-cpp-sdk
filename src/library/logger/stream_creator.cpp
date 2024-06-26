#include "stream_creator.h"
#include "stream.h"

#include <iostream>

THolder<TLogBackend> TCerrLogBackendCreator::DoCreateLogBackend() const {
    return MakeHolder<TStreamLogBackend>(&std::cerr);
}


TCerrLogBackendCreator::TCerrLogBackendCreator()
    : TLogBackendCreatorBase("cerr")
{}

void TCerrLogBackendCreator::DoToJson(NJson::TJsonValue& /*value*/) const {
}

ILogBackendCreator::TFactory::TRegistrator<TCerrLogBackendCreator> TCerrLogBackendCreator::RegistrarCerr("cerr");
ILogBackendCreator::TFactory::TRegistrator<TCerrLogBackendCreator> TCerrLogBackendCreator::RegistrarConsole("console");


THolder<TLogBackend> TCoutLogBackendCreator::DoCreateLogBackend() const {
    return MakeHolder<TStreamLogBackend>(&std::cout);
}


TCoutLogBackendCreator::TCoutLogBackendCreator()
    : TLogBackendCreatorBase("cout")
{}

ILogBackendCreator::TFactory::TRegistrator<TCoutLogBackendCreator> TCoutLogBackendCreator::Registrar("cout");

void TCoutLogBackendCreator::DoToJson(NJson::TJsonValue& /*value*/) const {
}
