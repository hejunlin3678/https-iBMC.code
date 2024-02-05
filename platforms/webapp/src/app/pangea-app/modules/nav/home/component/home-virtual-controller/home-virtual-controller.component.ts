import { Component, OnInit, ElementRef, ViewChild, Renderer2 } from '@angular/core';
import { Router } from '@angular/router';

import { TiModalService } from '@cloud/tiny3';

import { HomeService } from '../../services/home.service';
import { IVirtualButton, VirtualBtnId } from '../../home.datatype';
import { HomeVirtualModalComponent } from '../home-virtual-modal';
import { CommonService, UserInfoService, TimeoutService, LanguageService } from 'src/app/common-app/service';

@Component({
    selector: 'app-home-virtual-controller',
    templateUrl: './home-virtual-controller.component.html',
    styleUrls: ['./home-virtual-controller.component.scss']
})
export class HomeVirtualControllerComponent implements OnInit {
    public showOptions = false;
    public hasRight = false;
    public showImg = true;
    public loaded = false;
    public virtualUrl: string | ArrayBuffer;
    private kvmCheckTimer: any;
    private clientCheckTimer: any;
    private resizeTimer: any;
    private queryFailedTimes = 0;
    @ViewChild('imgRef', { static: false }) imgRef: ElementRef;
    @ViewChild('canvasRef', { static: false }) canvasRef: ElementRef;

    public virtualOptions: IVirtualButton[] = [
        {
            id: VirtualBtnId.h5Only,
            label: 'HOME_HTML5_KVM_ONLY',
            disabled: false,
            show: true,
        },
        {
            id: VirtualBtnId.h5Share,
            label: 'HOME_HTML5_KVM_SHARE',
            disabled: false,
            show: true,
        },
        {
            id: VirtualBtnId.javaOnly,
            label: 'HOME_JAVA_KVM_ONLY',
            disabled: false,
            show: true,
        },
        {
            id: VirtualBtnId.javaShare,
            label: 'HOME_JAVA_KVM_SHARE',
            disabled: false,
            show: true,
        },
        {
            id: VirtualBtnId.more,
            label: 'REMOTE_MORE_INFO',
            disabled: false,
            show: true,
        },
    ];

    constructor(
        private homeService: HomeService,
        private tiModal: TiModalService,
        private rd2: Renderer2,
        private router: Router,
        private commonService: CommonService,
        private userService: UserInfoService,
        private timeoutService: TimeoutService,
        private langService: LanguageService
    ) { }

    ngOnInit() {
        this.initKvm();
        window.addEventListener('resize', () => this.windowResize);
    }

    ngOnDestroy(): void {
        clearInterval(this.clientCheckTimer);
        clearInterval(this.kvmCheckTimer);
        window.removeEventListener('resize', () => this.windowResize);
    }

    public virtualClick(id: string) {
        const clickedBtn: IVirtualButton = this.virtualOptions.find((option: IVirtualButton) => option.id === id);
        if (clickedBtn.disabled) {
            return;
        }
        if (id === VirtualBtnId.h5Only) {
            this.openKVM('Private');
        }
        if (id === VirtualBtnId.h5Share) {
            this.openKVM('Shared');
        }
        if (id === VirtualBtnId.javaOnly) {
            this.downloadFile('Private');
        }
        if (id === VirtualBtnId.javaShare) {
            this.downloadFile('Shared');
        }
        if (id === VirtualBtnId.more) {
            this.openInforModal();
        }
    }

    public virtualStart() {
        if (!this.hasRight) {
            return;
        }
        this.showOptions = !this.showOptions;
    }

    public openKVMPage() {
        this.router.navigate(['/navigate/service/kvm']);
    }

    private openKVM(right: string): void {
        localStorage.setItem('H5ClientModel', right);
        // localH5KVMInfo缓存用于，在KVM页面刷新时调用，刷新会重连websocket，此时不能重新调接口，使用缓存数据，代表非法用户
        localStorage.setItem('localH5KVMInfo', null);
        const toURL = `#/kvm_h5`;
        const resourceId = this.userService.resourceId;
        if (!resourceId) {
            return;
        }

        this.homeService.openKVM(resourceId).subscribe((res) => {
            window.open(toURL, '_blank');
        }, () => { });
    }

    private downloadFile(right: string) {
        const params = {
            Language: this.langService.locale.split('-')[0],
            Mode: right
        };
        this.homeService.exportKvmStartupFile(params).subscribe((res) => {
            const data = res['body'];
            this.commonService.downloadFileWithResponse(data, 'kvm.jnlp');
        });
    }

    private openInforModal() {
        this.tiModal.open(HomeVirtualModalComponent, {
            id: 'home-virtual-infor',
        });
    }

    private initKvm() {
        this.hasRight = this.userService.hasPrivileges(['OemKvm']);
        if (!this.hasRight) {
            this.loaded = true;
            return;
        }
        this.continuedSearchKvmStatus();
    }


    private continuedSearchKvmStatus() {
        this.searchKvmActiveStatus(false);

        // 查询kvm的会话状态，10秒查一次
        this.kvmCheckTimer = setInterval(() => {
            this.searchKvmActiveStatus(true);
        }, 10000);

        // 如果报401的错误，则停止查询kvm会话状态
        this.timeoutService.error401.subscribe(() => {
            clearInterval(this.kvmCheckTimer);
        });
    }

    private searchKvmActiveStatus(timingRequest: boolean): void {
        this.homeService.searchKvmActiveStatus(timingRequest).subscribe((res) => {
            const data = res['body'];
            const visibleJavaKvm = data.VisibleJavaKvm;
            const activeStatus = data.ActivatedSessionsType || [];
            let disabledIds: string[] = [];
            if (data.MaximumNumberOfSessions === data.NumberOfActiveSessions) {
                // 置灰所有
                disabledIds = [VirtualBtnId.h5Only, VirtualBtnId.h5Share, VirtualBtnId.javaOnly, VirtualBtnId.javaShare];
            } else if (activeStatus.indexOf('Private') > -1) {
                // 置灰所有
                disabledIds = [VirtualBtnId.h5Only, VirtualBtnId.h5Share, VirtualBtnId.javaOnly, VirtualBtnId.javaShare];
            } else if (activeStatus.indexOf('Shared') > -1) {
                // 置灰两个独占
                disabledIds = [VirtualBtnId.h5Only, VirtualBtnId.javaOnly];
            }
            if (!visibleJavaKvm) {
                this.virtualOptions[2].show = visibleJavaKvm;
                this.virtualOptions[3].show = visibleJavaKvm;
            }
            for (const buttonOpt of this.virtualOptions) {
                const isDisabled = disabledIds.indexOf(buttonOpt.id) > -1;
                buttonOpt.disabled = isDisabled;
            }
        });

    }

    private getImg(url: string) {
        const promise = new Promise((resolve, reject) => {
            const xhr = new XMLHttpRequest();
            xhr.open('GET', url);
            xhr.responseType = 'blob';
            xhr.onerror = () => {
                reject();
            };
            xhr.onload = () => {
                if (xhr.status === 200) {
                    this.loaded = true;
                    const blob = xhr.response;
                    const fileReader = new FileReader();
                    fileReader.readAsDataURL(blob);
                    fileReader.onload = (e) => {
                        // ts判断e.target上没有result属性，所以做此处理
                        const event = e as any;
                        this.virtualUrl = event.target.result;
                        this.rd2.setStyle(this.imgRef.nativeElement, 'opacity', 1);
                        setTimeout(() => {
                            this.drawImg();
                        }, 100);
                    };
                    resolve(true);
                } else {
                    reject();
                }
            };
            xhr.send();
            // 提前捕获错误，这样Promise.all是所有请求都结束时触发
        }).catch(() => { });

        return promise;
    }

    private drawImg() {
        const imgEle = this.imgRef.nativeElement;
        if (!imgEle) {
            return;
        }

        const canvasEle = this.canvasRef.nativeElement;
        const ctx = canvasEle.getContext('2d');
        const imgWidth = window.getComputedStyle(imgEle).width;
        const imgHeight = window.getComputedStyle(imgEle).height;
        this.rd2.setStyle(canvasEle, 'width', imgWidth);
        this.rd2.setStyle(canvasEle, 'height', imgHeight);
        ctx.drawImage(imgEle, 0, 0, imgWidth, imgHeight);
    }

    private windowResize() {
        clearTimeout(this.resizeTimer);
        this.resizeTimer = setTimeout(() => {
            this.drawImg();
        }, 100);
    }

}
