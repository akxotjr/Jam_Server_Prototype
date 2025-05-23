@echo off
pushd %~dp0

set PROTOC_PATH=C:\Users\matae\vcpkg\installed\x64-windows\tools\protobuf

"%PROTOC_PATH%\protoc.exe" -I=./ --cpp_out=./ ./Enum.proto
"%PROTOC_PATH%\protoc.exe" -I=./ --cpp_out=./ ./Struct.proto
"%PROTOC_PATH%\protoc.exe" -I=./ --cpp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=C_ --send=S_
GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=S_ --send=C_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../GameServer"
XCOPY /Y Enum.pb.cc "../../../GameServer"
XCOPY /Y Struct.pb.h "../../../GameServer"
XCOPY /Y Struct.pb.cc "../../../GameServer"
XCOPY /Y Protocol.pb.h "../../../GameServer"
XCOPY /Y Protocol.pb.cc "../../../GameServer"
XCOPY /Y ClientPacketHandler.h "../../../GameServer"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE