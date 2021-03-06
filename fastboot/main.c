/*
 * Copyright (C) 2018 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 *
 * Authors: Bin Yang <bin.yang@intel.com>
 */
#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/sysmacros.h>
#include <pthread.h>

#define DEFAULT_INIT "/sbin/init"

#ifdef SPLASH_SCREEN_FB_FILE
void *splash_screen_init(void *arg);
#endif
static int splash_screen_trigger = 0;

#define CBC_ATTACH
#ifdef CBC_ATTACH
static pthread_t cbc_attach_tid;
void *cbc_attach_init(void *arg)
{
	int ret;
	ret = system("/usr/bin/cbc_attach");
	if (ret < 0)
		fprintf(stderr, "failed to cbc attach\n");
	return NULL;
}
#endif

#define ARRAY_SIZE(array)       (sizeof(array) / sizeof((array)[0]))
static pthread_t load_ipu4_modules_tid;
static pthread_t load_ipu4_crlmodule_lite_tid;
static pthread_t load_ipu4_isys_csslib_tid;
static pthread_t load_ipu4_psys_csslib_tid;
static pthread_t load_ipu4_mmu_tid;
static pthread_t load_ipu4_psys_tid;
static pthread_t load_ipu4_isys_tid;
char *ipu4_modulesp[]  = { 
	"crlmodule-lite",
	"intel-ipu4",
	"intel-ipu4-mmu",
	"intel-ipu4-psys",
	"ici-isys-mod",
	"intel-ipu4-psys-csslib",
	"intel-ipu4-isys-csslib",
};

void *load_ipu4_mmu(void *arg)
{
	int ret;
	char mprobe[64];
	sprintf(mprobe, "modprobe %s", ipu4_modulesp[2]);
	ret = system(mprobe);
	if (ret < 0)
		fprintf(stderr, "faile to modprobe %s", ipu4_modulesp[2]);
	return NULL;
}

void *load_ipu4_psys(void *arg)
{
	int ret;
	char mprobe[64];
	sprintf(mprobe, "modprobe %s", ipu4_modulesp[3]);
	ret = system(mprobe);
	if (ret < 0)
		fprintf(stderr, "faile to modprobe %s", ipu4_modulesp[3]);
	return NULL;
}


void *load_ipu4_isys(void *arg)
{
	int ret;
	char mprobe[64];
	sprintf(mprobe, "modprobe %s", ipu4_modulesp[4]);
	ret = system(mprobe);
	if (ret < 0)
		fprintf(stderr, "faile to modprobe %s", ipu4_modulesp[4]);
	return NULL;
}


void *load_ipu4_crlmodule_lite(void *arg)
{
	int ret;
	char mprobe[64];
	sprintf(mprobe, "modprobe %s", ipu4_modulesp[0]);
	ret = system(mprobe);
	if (ret < 0)
		fprintf(stderr, "faile to modprobe %s", ipu4_modulesp[0]);
	return NULL;
}

void *load_ipu4_isys_csslib(void *arg)
{
	int ret;
	char mprobe[64];
	sprintf(mprobe, "modprobe %s", ipu4_modulesp[6]);
	ret = system(mprobe);
	if (ret < 0)
		fprintf(stderr, "faile to modprobe %s", ipu4_modulesp[6]);
	return NULL;
}

void *load_ipu4_psys_csslib(void *arg)
{
	int ret;
	char mprobe[64];
	sprintf(mprobe, "modprobe %s", ipu4_modulesp[5]);
	ret = system(mprobe);
	if (ret < 0)
		fprintf(stderr, "faile to modprobe %s", ipu4_modulesp[5]);
	return NULL;
}

void *load_ipu4_modules(void *arg)
{
	char mprobe[64];
	int ret;

	sprintf(mprobe, "modprobe %s", ipu4_modulesp[1]);
	ret = system(mprobe);
	if (ret < 0) 
		fprintf(stderr, "faile to modprobe %s", ipu4_modulesp[1]);

	return NULL;
}

#ifdef EARLY_AUDIO_CMD
static pthread_t early_audio_tid;
void *setup_early_audio(void *arg)
{
	int ret = 0;
	ret = system(EARLY_AUDIO_CMD);
	if (ret < 0) {
		fprintf(stderr, "faile to setup early audio - %s",
				EARLY_AUDIO_CMD);
	}
	return NULL;
}
#endif

#ifdef PRELOAD_LIST_FILE
static pthread_t preload_tid;
static void *preload_thread(void *arg)
{
	int fd;
	struct stat sb;
	int ret;
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	fp = fopen(PRELOAD_LIST_FILE, "r");
	if (!fp) {
//		fprintf(stderr, "open preload list error (%d): %m\n", errno);
		goto exit;
	}

	while ((nread = getline(&line, &len, fp)) != -1) {
		if (nread < 2) {
			continue;
		}
		if (line[nread - 1] == '\n' || line[nread - 1] == '\r')
			line[nread - 1] = 0;
		fd = open(line, O_RDONLY);
		if (fd > 0) {
			fstat(fd, &sb);
			ret = readahead(fd, 0, sb.st_size);
			if (ret == 0) {
				//fprintf(stdout, "preload %s success\n", line);
			}else {
				fprintf(stdout, "preload %s error (%d): %m\n", line, errno);
			}
			close(fd);
		} else {
			//fprintf(stdout, "preload open %s error (%d): %m\n", line, errno);
		}
	}

exit:
	free(line);
	if (fp)
		fclose(fp);
	return NULL;
}
#endif

static int init_work(void)
{
	int ret;
	ret = system("mkdir -p " WORKDIR);
	if (ret < 0) {
		fprintf(stderr, "create dir %s error (%d): %m\n", WORKDIR, errno);
		return -1;
	}

	/* for kpi test */
	if (access("/sys/class/gpio/export", R_OK) != 0) {
		mount("/sys", "/sys", "sysfs", 0, NULL);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int fd;
	int ret;
	char buf[8];

	if (getpid() == 1) {
		fd = open(SPLASH_SCREEN_TRIGGER_FILE, O_RDONLY);
		if (fd > 0) {
			close(fd);
			init_work();
			splash_screen_trigger = 1;
			splash_screen_init(NULL);
		}
		if (fork())
			execl(DEFAULT_INIT, DEFAULT_INIT, NULL);
	}

    	if (0 == splash_screen_trigger){
		ret = init_work();
		if (ret < 0)
			return ret;
	}

    /* try to load ipu4 modules AEAP */
	pid_t pid = fork();
	if (pid < 0)
		fprintf(stderr, "fork ipu4 pid error\n");
        else if (pid == 0) {
		pthread_create(&load_ipu4_modules_tid, NULL, load_ipu4_modules, NULL);
		pthread_create(&load_ipu4_crlmodule_lite_tid, NULL, load_ipu4_crlmodule_lite, NULL);
		pthread_create(&load_ipu4_isys_csslib_tid, NULL, load_ipu4_isys_csslib, NULL);
		pthread_create(&load_ipu4_psys_csslib_tid, NULL, load_ipu4_psys_csslib, NULL);
		pthread_create(&load_ipu4_mmu_tid,  NULL, load_ipu4_mmu, NULL);
		pthread_create(&load_ipu4_psys_tid,  NULL, load_ipu4_psys, NULL);
		pthread_create(&load_ipu4_isys_tid,  NULL, load_ipu4_isys, NULL);
#ifdef CBC_ATTACH
		pthread_create(&cbc_attach_tid, NULL, cbc_attach_init, NULL);
#endif

#ifdef CBC_ATTACH
		pthread_join(cbc_attach_tid, NULL);
#endif

		pthread_join(load_ipu4_modules_tid, NULL);
		pthread_join(load_ipu4_crlmodule_lite_tid, NULL);
		pthread_join(load_ipu4_isys_csslib_tid, NULL);
		pthread_join(load_ipu4_psys_csslib_tid, NULL);
		pthread_join(load_ipu4_mmu_tid, NULL);
		pthread_join(load_ipu4_psys_tid,  NULL);
		pthread_join(load_ipu4_isys_tid,  NULL);
		return 0;
	}

#ifdef PRELOAD_LIST_FILE
	pthread_create(&preload_tid, NULL, preload_thread, NULL);
#endif

#ifdef EARLY_AUDIO_CMD
	pthread_create(&early_audio_tid, NULL, setup_early_audio, NULL);
#endif

#ifdef PRELOAD_LIST_FILE
	pthread_join(preload_tid, NULL);
#endif

#ifdef EARLY_AUDIO_CMD
	pthread_join(early_audio_tid, NULL);
#endif
	return 0;
}
