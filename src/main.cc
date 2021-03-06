/*
 * Copyright (c) 2014 loujiayu
 * This file is released under the MIT License
 * http://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <getopt.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "pcs.h"
#include "jsonentry.h"
#include "filetrans.h"
#include "filesystem.h"
#include "log.h"

 void ExitWithHelp() {
  printf(
    "Usage: baiduyun [option]\n"
    "options:\n"
    "-a authenicate:  authenticate your account with access token\n"
    "-d download:     download files from /apps/ldrive\n"
    "-u upload:       upload files from ./Baidu_Yun\n"
    "-s synchronise:  detect files need to be downloaded or uploaded and do it\n"
    "-h help:         help message\n");
  exit(-1);
 }

int main(int argc, char *argv[]) {
  using namespace by;
  log::LogFile fp(NULL);
  JsonEntry config = fp.ReadConfig();
  int c;
  bool download = false,upload = false,sync = false;
  const std::string kClientID  = "dOiFFnAqiGhzpsT19ijBqpaM";
  const std::string kClientSecret  = "QVCr57iC3g8AjX5pRlkbSPIrivAtY1BE";
  while ((c = getopt(argc, argv, "aduhs")) != -1) {
    switch (c) {
      default:
      case 'h': {
        ExitWithHelp();
        break;
      }
      case 'a': {
        std::cout
          << "-----------------------\n"
          << "Please go to this URL and get an authenication code:\n\n"
          << Pcs::AuthURL(kClientID)
          << std::endl;

        std::cout
          << "\n-----------------------\n"
          << "Please input the authenication code here" << std::endl;
        std::string code;
        std::cin >> code;

        Pcs baidu_oauth(kClientID, kClientSecret);
        baidu_oauth.Auth(code);
        config.Add("access_token", JsonEntry(baidu_oauth.access_token()));
        assert(config["access_token"].Value<std::string>() ==
          baidu_oauth.access_token());
        fp.SaveConfig(config);
        break;
      }
      case 's': {
        sync = true;
        break;
      }
      case 'd': {
        download = true;
        break;
      }
      case 'u': {
        upload = true;
        break;
      }

    }
  }

  std::string access_token;
  try {
    access_token = (config["access_token"].Value<std::string>());
  }
  catch(const std::runtime_error& error) {
    fprintf (stderr,"The first time run baiduyun,please add option \"-a\" to authenticate your account");
    return -1;
  }
  FileTrans ft(access_token);
  if(download)
    ft.Downloads(kLocalRoot);
  else if(upload)
    ft.Uploads(kLocalRoot);
  else if(sync)
    ft.Drive(kLocalRoot);
  return 0;
}
