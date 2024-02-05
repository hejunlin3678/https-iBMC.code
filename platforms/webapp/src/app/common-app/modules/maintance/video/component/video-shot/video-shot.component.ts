import { Component, Input, OnChanges, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { AlertMessageService, UserInfoService, CommonService, ErrortipService, LanguageService } from 'src/app/common-app/service';
import { VideoService } from '../../services';
import { TiMessageService } from '@cloud/tiny3';
import { getRandomFileName } from 'src/app/common-app/utils';

@Component({
    selector: 'app-video-shot',
    templateUrl: './video-shot.component.html',
    styleUrls: ['../../video.component.scss', './video-shot.component.scss']
})
export class VideoShotComponent implements OnChanges, OnInit {

    constructor(
        private translate: TranslateService,
        private langService: LanguageService,
        private alert: AlertMessageService,
        private videoService: VideoService,
        private tiMessage: TiMessageService,
        private commonService: CommonService,
        private errorTipService: ErrortipService,
        private user: UserInfoService
    ) { }

    @Input() shotData;
    @Input() isSystemLock;

    public languageToken = this.langService.activeLanguage.tiny3Set.toLowerCase();
    public shotSwitch = {
        Enable: null,
        change: () => {
            // 录像使能方法
            const instance = this.tiMessage.open({
                id: 'shotConfirm',
                type: 'prompt',
                content: this.translate.instant('COMMON_ASK_OK'),
                okButton: {
                    show: true,
                    text: this.translate.instant('ALARM_OK'),
                    autofocus: false,
                    click: () => {
                        let queryParams = {};
                        queryParams = {
                            'VideoRecordingEnabled': !this.shotSwitch.Enable
                        };
                        this.call(queryParams);
                        instance.close();
                    }
                },
                cancelButton: {
                    show: true,
                    text: this.translate.instant('ALARM_CANCEL'),
                    click: () => {
                        instance.dismiss();
                    }
                },
                title: this.translate.instant('ALARM_OK'),
            });
        }
    };

    public captureVideooGroup = [
        {
            url: '',
            label: this.translate.instant('VIDEO_RECORDING_LOCAL'),
            detail: this.translate.instant('VIDEO_DOWNLOAD_PLAYER'),
            value: '0',
            downloadId: '',
            playId: 'localId',
            active: false,
            playerMenu: {
                eId: 'playerMenuId',
                disable: false,
                placeholder: this.translate.instant('DASH_START_VIRMON_STATION'),
                options: [{
                    id: 0,
                    label: this.translate.instant('HTML5_VIDEO_PLAYER'),
                    active: false,
                    playId: 'localId'
                },
                {
                    id: 1,
                    label: this.translate.instant('JAVA_VIDEO_PLAYER'),
                    active: false,
                    playId: 'localId'
                }],
                change: (ev, opt) => {
                    ev.stopPropagation();
                    this.openPlayer(opt.id, opt.playId);
                }
            }
        },
        {
            url: '',
            label: this.translate.instant('VIDEO_RECORDING_CPU_ERROR'),
            detail: this.translate.instant('VIDEO_NO_VIDEO'),
            value: '0',
            downloadId: 'cpuErrorId',
            playId: 'cpuVedioId',
            active: false,
            playerMenu: {
                eId: 'playerMenuId',
                disable: false,
                placeholder: this.translate.instant('DASH_START_VIRMON_STATION'),
                options: [{
                    id: 0,
                    label: this.translate.instant('HTML5_VIDEO_PLAYER'),
                    active: false,
                    playId: 'cpuVedioId'
                },
                {
                    id: 1,
                    label: this.translate.instant('JAVA_VIDEO_PLAYER'),
                    active: false,
                    playId: 'cpuVedioId'
                }
                ],
                change: (ev, opt) => {
                    ev.stopPropagation();
                    this.openPlayer(opt.id, opt.playId);
                }
            }
        },
        {
            url: '',
            label: this.translate.instant('VIDEO_RECORDING_SHUTDOWN'),
            detail: this.translate.instant('VIDEO_NO_VIDEO'),
            value: '0',
            downloadId: 'downOffId',
            playId: 'offVedioId',
            active: false,
            playerMenu: {
                eId: 'playerMenuId',
                disable: false,
                placeholder: this.translate.instant('DASH_START_VIRMON_STATION'),
                options: [{
                    id: 0,
                    label: this.translate.instant('HTML5_VIDEO_PLAYER'),
                    active: false,
                    playId: 'offVedioId'
                },
                {
                    id: 1,
                    label: this.translate.instant('JAVA_VIDEO_PLAYER'),
                    active: false,
                    playId: 'offVedioId'
                }
                ],
                change: (ev, opt) => {
                    ev.stopPropagation();
                    this.openPlayer(opt.id, opt.playId);
                }
            }
        },
        {
            url: '',
            label: this.translate.instant('VIDEO_RECORDING_RESTART'),
            detail: this.translate.instant('VIDEO_NO_VIDEO'),
            value: '0',
            downloadId: 'restDownId',
            playId: 'restVedioId',
            active: false,
            playerMenu: {
                eId: 'playerMenuId',
                disable: false,
                placeholder: this.translate.instant('DASH_START_VIRMON_STATION'),
                options: [{
                    id: 0,
                    label: this.translate.instant('HTML5_VIDEO_PLAYER'),
                    active: false,
                    playId: 'restVedioId'
                },
                {
                    id: 1,
                    label: this.translate.instant('JAVA_VIDEO_PLAYER'),
                    active: false,
                    playId: 'restVedioId'
                }
                ],
                change: (ev, opt) => {
                    ev.stopPropagation();
                    this.openPlayer(opt.id, opt.playId);
                }
            }
        }
    ];

    public shotInit = (shotData) => {
        this.shotSwitch.Enable = shotData.VideoRecordingEnabled;
        const videoCreate = shotData.VideoRecordInfo;
        if (videoCreate && videoCreate.length > 0) {
            for (let i = 0; i < videoCreate.length; i++) {
                this.captureVideooGroup[i + 1].value = null !== videoCreate[i]
                    ? this.getFileSize(videoCreate[i].VideoSizeByte)
                    : '0';
                if (this.captureVideooGroup[i + 1].value !== '0') {
                    this.captureVideooGroup[i + 1].detail = videoCreate[i].CreateTime;
                }
            }
        }
    }

    ngOnInit() {
        window.addEventListener('mousedown', this.widnowDown , false);
    }

    private widnowDown = (ev) => {
        const target: HTMLElement = ev.target;
        const className = target.getAttribute('class');
        if (className && className.indexOf('circleWrite2') === -1 && className.indexOf('playMenu') === -1) {
            for (const key of this.captureVideooGroup) {
                key.active = false;
            }
        }
    }

    ngOnDestroy(): void {
        window.removeEventListener('mousedown', this.widnowDown);
    }


    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        if (changes.shotData && changes.shotData.currentValue) {
            this.shotInit(changes.shotData.currentValue);
        }
    }

    public call = (queryParams) => {
        this.videoService.setOnOffVal(JSON.stringify(queryParams)).subscribe((response) => {
            this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
            this.shotData = response.body;
            this.shotInit(this.shotData);
        }, (error) => {
            this.alert.eventEmit.emit({type: 'error', label: 'COMMON_FAILED'});
            this.shotData = error.error.data;
            this.shotInit(this.shotData);
        });
    }

    public getFileSize = (fileSize) => {
        let result;
        result = 0;
        if (typeof fileSize === 'number') {
            let size;
            size = '';
            if (fileSize < 1024) {
                size = fileSize;
                result = '(' + size + ' Byte)';
            }
            if (fileSize >= 1024 && fileSize < 1024 * 1024) {
                size = (fileSize / 1024).toFixed(3);
                result = '(' + size + ' KB)';
            }

            if (fileSize >= 1024 * 1024) {
                size = (fileSize / (1024 * 1024)).toFixed(3);
                result = '(' + size + ' MB)';
            }
        }
        return result;
    }

    public downloadPlayer = (event, playId) => {
        if (!this.user.hasPrivileges(['OemDiagnosis'])) {
            return;
        }
        for (const key of this.captureVideooGroup) {
            if (key.playId === playId) {
                key.active = true;
            } else {
                key.active = false;
            }
            event.stopPropagation();
            event.preventDefault();
        }
    }

    public downloadVideo = (downloadId) => {
        const queryParams = {
            // 支持导出到本地或远端
            'VideoType': ''
        };

        switch (downloadId) {
            case 'cpuErrorId':
                queryParams.VideoType = 'CaterrorVideo';
                break;
            case 'downOffId':
                queryParams.VideoType = 'PoweroffVideo';
                break;
            case 'restDownId':
                queryParams.VideoType = 'OsresetVideo';
                break;
            default:
                queryParams.VideoType = '';
                break;
        }
        this.downloadFN(queryParams);
    }

    public openPlayer = (playMethod, playId) => {
        for (const key of this.captureVideooGroup) {
            if (key.playId === playId) {
                key.active = false;
            }
        }
        this.videoService.videoScreenCapture().subscribe((res) => {
            const queryParams = {
                'VideoType': '',
                'ServerName': window.location.host,
                'Language': this.languageToken.split('-')[0],
                'HostName': window.location.host
            };

            switch (playId) {
                case 'localId':
                    queryParams.VideoType = 'LocalVideo';
                    break;
                case 'cpuVedioId':
                    queryParams.VideoType = 'CaterrorVideo';
                    break;
                case 'offVedioId':
                    queryParams.VideoType = 'PoweroffVideo';
                    break;
                case 'restVedioId':
                    queryParams.VideoType = 'OsresetVideo';
                    break;
                default:
                    queryParams.VideoType = '';
            }

            // 非本地播放器且有其他用户占用，则需要先踢出用户
            if (playId !== 'localId' && res.body.VideoPlaybackConnNum > 0) {
                const instance1 = this.tiMessage.open({
                    id: 'videoConfirm',
                    type: 'prompt',
                    content: this.translate.instant('VIDEO_DEL_VIDEO_USER'),
                    okButton: {
                        show: true,
                        text: this.translate.instant('ALARM_OK'),
                        autofocus: false,
                        click: () => {
                            instance1.close();
                            this.videoService.stopvideoplayback({}).subscribe((resStop) => {
                                this.exportVideo(queryParams, playMethod, playId);
                            });
                        }
                    },
                    cancelButton: {
                        show: true,
                        text: this.translate.instant('ALARM_CANCEL'),
                        click(): void {
                            instance1.dismiss();
                        }
                    },
                    title: this.translate.instant('ALARM_OK'),
                });
            } else {
                this.exportVideo(queryParams, playMethod, playId);
            }
        });
    }

    public exportVideo = (queryParams, playMethod, playId) => {
        const constUrl = '/tmp/web/' + getRandomFileName('video.jnlp');
        const fileName = constUrl.split('/').slice(-1)[0];
        if (playMethod === 1) {
            queryParams.PlayType = 'Java';
            this.videoService.exportJavaVideoStartupFile(queryParams).subscribe((res) => {
                this.commonService.downloadFileWithResponse(res.body, fileName);
            }, (error) => {
                // 下载时blob报错没有错误对象，请求一次获取错误信息
                this.videoService.exportJavaVideoStartupFail(queryParams).subscribe((res) => {
                });
            });
        } else {
            localStorage.setItem('videoId', playId);
            queryParams.PlayType = 'HTML5';
            this.videoService.exportHtml5VideoStartupFile(queryParams).subscribe((response) => {
                localStorage.setItem('localH5VideoInfo', response['body']);
                localStorage.setItem('isValidVideoUser', 'true');
                const toURL = `#/videoPlayer/${playId}`;
                window.open(toURL);
            });
        }
    }

    // 下载录像文件到、tmp/web
    public downloadFN = (params) => {
        this.videoService.exportvideo(params).subscribe((res) => {
            this.taskService(res.body.url, params);
        }, (error) => {
            this.alert.eventEmit.emit({type: 'error', label: 'COMMON_FAILED'});
        });
    }

    // 下载文件步骤二 查询创建文件目录进度  Completed 为完成
    public taskService = (url, params) => {
        this.videoService.queryTask(url).subscribe((res) => {
            const taskData = res['body'];
            if (taskData.prepare_progress === 100 && taskData.downloadurl) {
                const downloadurl = taskData.downloadurl;
                // 下载
                this.commonService.restDownloadFile(downloadurl);
            } else if (taskData.prepare_progress <= 100) {
                setTimeout(() => {
                    this.taskService(url, params);
                }, 3000);
            }
        });
    }
}
