cmake_minimum_required(VERSION 3.5)
project(semver C)

add_library(${PROJECT_NAME} semver.h semver.c)