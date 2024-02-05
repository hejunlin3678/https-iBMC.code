import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import {run} from './classes/client';
import { TranslateService } from '@ngx-translate/core';
import {TiMessageService} from '@cloud/tiny3';
@Component({
    selector: 'app-video-player',
    templateUrl: './video-player.component.html',
    styleUrls: ['./video-player.component.scss']
})
export class VideoPlayerComponent implements OnInit {
    constructor(
        private translate: TranslateService,
        private tiMessage: TiMessageService
    ) {
        this.init();
     }
    @ViewChild('cvs') canvasRef: ElementRef;
    public scope = {
        videoParams: {},
        title: '',
        name: '',
        fileName: '',
        IP: '',
        SN: '',
        cvx: null,
        ctx: null,
        rightMenu: null,
        slider: null,
        isInited: false
    };

    public playStatus = 'VIDEO_PAUSE';
    public playId = localStorage.getItem('videoId');
    public init() {
        this.scope.rightMenu = {
            eId: 'rightMenu',
            disable: false,
            options: [
                { id: 0, label: 'VIDEO_FULLSCREEN', active: false, show: true },
                { id: 1, label: 'VIDEO_LOOP', active: false, show: false },
                { id: 2, label: 'VIDEO_PAUSE', active: false, show: true },
                { id: 3, label: 'VIDEO_OPEN', active: false, show: false },
                { id: 4, label: 'VIDEO_CAPTURE', active: false, show: true }
            ],
            change: (opt) => {
            }
        };
        this.scope.slider = {
            disabled:  false,
            value1: 0,
            tipMode:  'auto',
            min:  0,
            max:  100,
            scales: [],
            tipFormatterFn(value: number): number {
                return value;
            },
            change: (ev) => {},
            changeStop: (ev) => {}
        };
    }
    ngAfterViewInit(): void {
        this.scope.cvx = this.canvasRef.nativeElement;
        this.scope.ctx = this.scope.cvx.getContext('2d');
        run(this.scope, this.showMessage);
    }
    ngOnInit() {
        document.oncontextmenu =  (e) => {
            return false;
        };
        if (localStorage.getItem('isValidVideoUser') !== 'true') {
            this.showMessage('VIDEO_REMOTE_ERR_INVALID_USER');
            return;
        }
        localStorage.setItem('isValidVideoUser', 'false');
        switch (this.playId) {
            case 'localId':
                this.scope.fileName = '';
                this.scope.rightMenu.options[1].show = true;
                this.scope.rightMenu.options[3].show = true;
                break;
            case 'cpuVedioId':
                this.scope.fileName = 'video_caterror.rep';
                break;
            case 'offVedioId':
                this.scope.fileName = 'video_poweroff.rep';
                break;
            case 'restVedioId':
                this.scope.fileName = 'video_osreset.rep';
                break;
        }
        if (localStorage.getItem('localH5VideoInfo')) {
            this.scope.videoParams = JSON.parse(localStorage.getItem('localH5VideoInfo'));
            this.scope['title'] = this.scope.videoParams['title'].split('   ');
            this.scope['name'] = this.scope.title[0];
            this.scope['IP'] = this.scope.title[1];
            this.scope['SN'] = this.scope.title[2];
        }
    }

    public showMessage = (contentInfo) => {
        const instance = this.tiMessage.open({
            type: 'warn',
            id: 'videoWarnModal',
            content: this.translate.instant(contentInfo),
            title: this.translate.instant('VIDEO_HOME_PERFWARNING'),
            okButton: {
                show: true,
                text: this.translate.instant('COMMON_OK'),
                disabled: false,
                autofocus: false,
                click() {
                    instance.close();
                }
            },
            cancelButton: {
                show: false
            }
        });
    }
}
