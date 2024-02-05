import { Component, Input, OnChanges, ElementRef, Renderer2 } from '@angular/core';
import { TiMessageService } from '@cloud/tiny3';
import { VideoService } from '../../services';
import { AlertMessageService, UserInfoService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import Date from 'src/app/common-app/utils/date';

@Component({
    selector: 'app-video-picture',
    templateUrl: './video-picture.component.html',
    styleUrls: ['../../video.component.scss', './video-picture.component.scss']
})
export class VideoPictureComponent implements OnChanges {

    constructor(
        private tiMessage: TiMessageService,
        private videoService: VideoService,
        private user: UserInfoService,
        private alert: AlertMessageService,
        private elRef: ElementRef,
        private translate: TranslateService,
        private render2: Renderer2,
    ) { }

    @Input() captureData;
    @Input() isSystemLock;


    public picSwitch = {
        Enable: null,
        change: () => {
            const instance = this.tiMessage.open({
                id: 'picButton',
                type: 'prompt',
                content: this.translate.instant('COMMON_ASK_OK'),
                okButton: {
                    show: true,
                    text: this.translate.instant('ALARM_OK'),
                    autofocus: false,
                    click: () => {
                        let queryParams = {};
                        queryParams = {
                            'ScreenshotEnabled': !this.picSwitch.Enable
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

    public picCover = false;
    public bigsImg = '';
    public bigimgShow = false;
    public captFlag = true;
    public captrueFlag = false;
    public screenShotTime;
    public imageArray = [];
    public picCapture: boolean = false;
    public arrGroup = [
        {
            status: false,
            id: 'img0',
            isNewInfo: false,
            date: '',
            img: 'assets/common-assets/image/video/video_screen_shot_default.png'
        },
        {
            status: false,
            id: 'img1',
            isNewInfo: false,
            date: '',
            img: 'assets/common-assets/image/video/video_screen_shot_default.png'
        },
        {
            status: false,
            id: 'img2',
            isNewInfo: false,
            date: '',
            img: 'assets/common-assets/image/video/video_screen_shot_default.png'
        }
    ];

    // 排序
    public imgSort(arr: object[]) {
        if (arr.length === 0) {
            return arr;
        }
        const tmpArr = [];
        arr.forEach((item, idx) => {
            tmpArr.push({
                key: idx + 1,
                value: item
            });
        });
        tmpArr.sort((a, b) => {
            const newLast = new Date(b.value.replace(/-/g, '/'));
            const newBefore = new Date(a.value.replace(/-/g, '/'));
            return newLast - newBefore;
        });
        return tmpArr;
    }
    public captureInit = (captureData) => {
        if (!captureData) {
            return;
        }
        this.picSwitch.Enable = captureData.ScreenshotEnabled;
        this.imageArray = captureData.ScreenshotCreateTime;
        if (this.imageArray && JSON.stringify(this.imageArray) !== '{}') {
            const count = 3;
            const arrNew = [];
            for (let i = 0; i < count; i++) {
                arrNew.push(this.imageArray[`Time${i}`]);
            }
            const videoImgSort = this.imgSort(Array.from(arrNew));
            for (let i = 0; i < videoImgSort.length; i++) {
                if (videoImgSort[i].value === '' || videoImgSort[i].value == null) {
                    this.arrGroup[i].img = 'assets/common-assets/image/video/video_screen_shot_default.png';
                } else {
                    this.arrGroup[i].img = `/bmc/tmpshare/tmp/web/img${videoImgSort[i].key}.jpeg`;
                    this.arrGroup[i].date = videoImgSort[i].value;
                    this.arrGroup[i].status = true;
                }
            }
            // 首次进来查询一次是否有手动截屏
            const curURL = window.location.href;
            const curURLArr = curURL.split('#')[0];
            const displayImgURLJpeg = curURLArr.substr(0, curURLArr.lastIndexOf('/'))
                + '/bmc/tmpshare/tmp/web/manualscreen.jpeg';
            if (this.imageArray['Time3'] !== '' && this.imageArray['Time3'] !== null) {
                this.captrueFlag = true;
                this.getImg(displayImgURLJpeg);
            } else {
                this.captrueFlag = false;
            }
        }
    }

    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        if (changes.captureData && changes.captureData.currentValue) {
            this.captureInit(changes.captureData.currentValue);
        }
    }

    public call = (queryParams) => {
        this.videoService.setOnOffVal(JSON.stringify(queryParams)).subscribe((response) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.captureData = response['body'];
            this.captureInit(this.captureData);
        }, (error) => {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
            this.captureData = error.error.data;
            this.captureInit(this.captureData);
        });
    }

    // 虚拟控制台手动截屏
    public getImg = (url) => {
        let xmlhttp;
        xmlhttp = new XMLHttpRequest();
        xmlhttp.open('GET', url);
        xmlhttp.responseType = 'blob';
        xmlhttp.onload = () => {
            if (xmlhttp.status === 200) {
                this.elRef.nativeElement.querySelector('#captId').
                    setAttribute('src', '/bmc/tmpshare/tmp/web/manualscreen.jpeg');
                this.picCover = true;
                this.screenShotTime = this.imageArray['Time3'];
            }
        };
        xmlhttp.send();
    }

    // 开始手动截屏
    public capFN = () => {
        const instance1 = this.tiMessage.open({
            id: 'capScreen',
            type: 'prompt',
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
                click: () => {
                    instance1.close();
                    this.captrueScreenFn();
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
    }

    // 删除截屏
    public deleteFN = () => {
        this.captFlag = true;
        const instance2 = this.tiMessage.open({
            id: 'delScreen',
            type: 'prompt',
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
                click: () => {
                    instance2.close();
                    this.deleteCapt();
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('ALARM_CANCEL'),
                click(): void {
                    this.captrueFlag = true;
                    instance2.dismiss();
                }
            },
            title: this.translate.instant('ALARM_OK'),
        });
    }

    public captrueScreenFn = () => {
        this.videoService.capturescreenshot({}).subscribe((res) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            if (res.body !== undefined && res.body !== null) {
                this.taskFn(res.body.url);
            }
        });
    }

    // 自动截屏的那三个图片的事件
    public imgFN = (imgId, item?) => {
        if (imgId === 'captId' && !this.captrueFlag) {
            this.captrueFlag = true;
            return;
        }
        this.bigimgShow = true;
        this.imgShow('#imgs', this.elRef.nativeElement.querySelector('#' + imgId), item);
    }

    // 查询进度
    public taskFn = (url) => {
        this.picCapture = true;
        this.videoService.queryTask(url).subscribe((res) => {
            const taskData = res['body'];
            if (taskData !== undefined) {
                if (taskData.State === 'Completed') {
                    this.elRef.nativeElement.querySelector('#captId').
                        setAttribute('src', '/bmc/tmpshare/tmp/web/manualscreen.jpeg?a=' + Date.now());
                    this.picCover = true;
                    this.screenShotTime = (new Date(taskData.ScreenshotCreateTime.substr(0, 19))).pattern('yyyy-MM-dd HH:mm:ss');
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    this.picCapture = false;
                    this.videoService.videoScreenCapture().subscribe((data) => {
                        this.captureData = data.body;
                        this.captureInit(this.captureData);
                    }, () => {
                    });
                } else {
                    setTimeout(() => {
                        this.taskFn(url);
                    }, 3000);
                }
            }
        }, () => {
            this.picCapture = false;
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
        });
    }

    // 删除截屏
    public deleteCapt = () => {
        this.videoService.deletescreenshot({}).subscribe((res) => {
            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.elRef.nativeElement.querySelector('#captId').setAttribute('src', 'assets/common-assets/image/video/u9406.png');
            this.picCover = false;
            this.captrueFlag = false;
        }, (error) => {
            this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
        });
    }

    // 实现点击图片在弹出层显示大图
    public imgShow = (bigimg, _this, item) => {
        let src;
        if (item) {
            src = item.img;
        } else {
            src = _this.getAttribute('src');
        }
        // 设置#bigimg元素的src属性
        this.bigsImg = src;

        // 获取当前窗口宽度
        const windowW = window.innerWidth;
        // 获取当前窗口高度
        const windowH = window.innerHeight;
        // 获取图片真实宽度
        const realWidth = 1024;
        // 获取图片真实高度
        const realHeight = 768;
        let imgWidth;
        let imgHeight;
        // 缩放尺寸，当图片真实宽度和高度大于窗口宽度和高度时进行缩放
        const scale = 0.8;
        if (realHeight > windowH * scale) {
            /*
            * 判断图片高度
            * 如大于窗口高度，图片高度进行缩放
            */
            imgHeight = windowH * scale;
            // 等比例缩放宽度
            imgWidth = imgHeight / realHeight * realWidth;
            if (imgWidth > windowW * scale) {
                /*
                    * 如宽度扔大于窗口宽度
                    * 再对宽度进行缩放
                    */
                imgWidth = windowW * scale;
            }
        } else if (realWidth > windowW * scale) {
            /*
                * 如图片高度合适，判断图片宽度
                * 如大于窗口宽度，图片宽度进行缩放
                */
            imgWidth = windowW * scale + 100;
            // 等比例缩放高度
            imgHeight = imgWidth / realWidth * realHeight + 100;
        } else {
            // 如果图片真实高度和宽度都符合要求，高宽不变
            imgWidth = realWidth + 100;
            imgHeight = realHeight + 100;
        }
        // 以最终的宽度对图片缩放
        setTimeout(() => {
            this.render2.setStyle(this.elRef.nativeElement.querySelector(bigimg), 'width', imgWidth + 'px');
        }, 100);
    }
    public bigImgFN = () => {
        this.bigimgShow = false;
    }

}
