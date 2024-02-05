import { Component, OnInit } from '@angular/core';
import { VideoService } from './services';
import { LoadingService, UserInfoService } from 'src/app/common-app/service';

@Component({
    selector: 'app-video',
    templateUrl: './video.component.html',
    styleUrls: ['./video.component.scss']
})
export class VideoComponent implements OnInit {

    constructor(
        private videoService: VideoService,
        private loadingService: LoadingService,
        private user: UserInfoService
    ) { }

    public isSystemLock = this.user.systemLockDownEnabled;

    public shotData = {};
    public shotGetInit = () => {
        this.videoService.videoScreenCapture().subscribe((res) => {
            this.shotData = res.body;
            this.loadingService.state.next(false);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.shotGetInit();
    }

}
