import { Router } from '@angular/router';
import { Component, OnInit, Input } from '@angular/core';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { IrmGlobalDataService } from 'src/app/irm-app/service/irm-global-data.service';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { HomeService } from '../../services';
import { TranslateService } from '@ngx-translate/core';
import { SystemLockService } from 'src/app/common-app/service/systemLock.service';
import { Monitor, TickMark, Baffle } from '../../model';
import { htmlEncode } from 'src/app/common-app/utils/common';
import { AlertMessageService } from 'src/app/common-app/service/alertMessage.service';
import { ErrortipService } from 'src/app/common-app/service/errortip.service';
import { CommonData } from 'src/app/common-app/models';

@Component({
    selector: 'app-irm-home-cabinet',
    templateUrl: './home-irm-cabinet.component.html',
    styleUrls: ['./home-irm-cabinet.component.scss']
})
export class HomeIrmCabinetComponent implements OnInit {

    @Input() overViewCabinetData: any;

    constructor(
        private homeService: HomeService,
        private userService: UserInfoService,
        private translate: TranslateService,
        private globalData: GlobalDataService,
        private irmGlobalData: IrmGlobalDataService,
        private router: Router,
        private alert: AlertMessageService,
        private errorTip: ErrortipService,
        private lockService: SystemLockService
    ) { }

    // irm现在不包含RM110系列产品，注释掉该处以免判断错误
    public isRM110: boolean = false;
    public jgUrl = this.isRM110 ? '/assets/irm-assets/image/home/jg.png' : '/assets/irm-assets/image/home/jg210.png';
    public hasConfigureComponentsRight = this.userService.hasPrivileges(['ConfigureComponents']);
    public systemLocked: boolean = false;

    public infoTips: any = {
        UIDTip: this.translate.instant('IRM_TAG_UID'),
        deviceModelTip: this.translate.instant('IRM_DEVICE_MODEL'),
        deviceTypeTip: this.translate.instant('COMMON_DEVICE_TYPE'),
        manufacturerTip: this.translate.instant('IRM_VENDOR'),
        uHeightTip: this.translate.instant('IRM_POSITION_USE'),
        serialNumberTip: this.translate.instant('IRM_AUTH_NUMBER'),
        partNumberTip: this.translate.instant('IRM_AUTH_NUMBER'),
        lifeCircleTip: this.translate.instant('ASSET_LIFE_CYCLE'),
        discoveredTimeTip: this.translate.instant('IRM_DISCOVER_TIME'),
        checkInTimeTip: this.translate.instant('IRM_CHECKIN_TIME'),
        weightTip: this.translate.instant('ASSET_WEIGHT'),
        ratedPowerWattsTip: this.translate.instant('ASSET_RATED_POWER_WATTS'),
        assetOwnerTip: this.translate.instant('IRM_ASSET_OWNER')
    };
    public statusTips: any = {
        normal: this.translate.instant('IRM_HOME_NORMAL'),
        locating: this.translate.instant('IRM_HOME_LOCATING'),
        alarm: this.translate.instant('IRM_HOME_ALARM'),
        notInPosition: this.translate.instant('IRM_HOME_NO_IN_POSITION')
    };

    // 机柜数据
    public srcDataAssetTag: any;
    // 资产信息表格数据
    public srcDataZc: any;
    public cabinetInfo: any = {};
    // 机柜丝印方向设置
    public alertMessage: any = {
        flag: true,
        complete: false
    };
    // 当前机柜丝印的方向
    public topToBottom: boolean = false;
    public unitTopToBottom: boolean = false;
    // 健康状态中的资产条状态
    public zcLevel: number = -1;
    // 刻度数据列表
    public graduation: any = [];
    // 机柜U位数据
    public actualAseetData: any = [];
    public selectedDevice: any = {};
    // 机柜图固定高度533
    public H_CABINET: number = 533;
    // 根据不同的值来确定占用的U位个数
    public TYPE_BOARD: any = {
        '0x0001': 6,
        '0x0002': 5
    };
    public uCount: any;
    public direction: any;
    // U位灯上的 mouseenter、mouseleave事件timer
    public timer: any;

    public loadingDirect: boolean = false;
    public hasBoards: boolean = false;
    // U位是否可以编辑
    public hasEditRole: boolean = false;
    // 当前选中的资产标签数据
    public currentDeviceTag: any = {};
    public cabinetNum: any = {
        green: 0,
        yellow: 0,
        red: 0,
        grey: 0
    };

    ngOnInit() {
        // 初始化一个当前选中的U位数据结构
        this.selectedDevice = new Baffle(0, 0);
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.systemLocked !== res) {
                this.systemLocked = res;
            }
        });
    }

    ngOnChanges() {
        this.initAsset(this.overViewCabinetData);
    }

    private initAsset(overViewCabinetData: any): void {
        if (overViewCabinetData) {
            this.getCabinetInfo(overViewCabinetData);
            this.getAllAssetTagData(overViewCabinetData);
        }
    }

    private getCabinetInfo(overViewCabinetData: any): void {
        // 总U位数UcountUsed
        this.cabinetInfo.TotalUCount = overViewCabinetData.monitorInfo.totalU;
        this.cabinetInfo.DirectionType = overViewCabinetData.Summary.RackInformation.DirectionType || 'Fixed';
        // 保存机柜丝印方向，Direction:丝印增长方向:FromTopDown代表上--->下，FromBottomUp代表下--->上
        this.topToBottom = overViewCabinetData.Summary.RackInformation.Direction === 'FromTopDown' ? true : false;
        this.unitTopToBottom = overViewCabinetData.Summary.RackInformation.UnitOccupyDirection === 'Downward' ? true : false;

        const dbData = this.drawGragAndBaffle(this.cabinetInfo.TotalUCount, overViewCabinetData.Summary.RackInformation.Direction);
    }



    // 根据机柜信息U位总数画图（刻度条和机柜挡板）
    private drawGragAndBaffle(uCount: any, direction: any) {
        if (!uCount) {
            return [];
        }

        const h = this.H_CABINET / uCount;

        // 刻度线
        const tempGrad: any = [];
        // 挡板
        const tempAsset: any = [];
        for (let k = 1; k <= uCount; k++) {
            const uNum = this.topToBottom ? k : uCount - k + 1;
            // 绘制刻度线
            tempGrad.push(new TickMark(uNum, h));

            // 绘制机柜内的挡板
            tempAsset.push(new Baffle(uNum, h));
        }

        // 做IF判断的原因是为了每次刷新不用重新绘制
        if (uCount !== this.uCount) {
            // 刻度线
            this.graduation = tempGrad;
        }

        // 做IF判断的原因是为了每次刷新时根据方向重新绘制刻度线
        if (!this.direction) {
            // 挡板数据
            this.actualAseetData = tempAsset;
        } else if (this.direction && (direction !== this.direction)) {
            // 反转刻度对应U位号
            this.actualAseetData.forEach((item, index, arr) => {
                item.uNum = arr.length - item.uNum + 1;
            });
        }

        // 保存当前的U位数
        this.uCount = uCount;
        // 保存当前的方向
        this.direction = direction;
        return tempAsset;
    }

    public sortBy(arr: any, name: string, sort: boolean) {
        if (arr.length === 0) {
            return arr;
        }
        arr.sort((a, b) => {
            if (sort) {
                return b[name] - a[name];
            } else {
                return a[name] - b[name];
            }
        });

        return arr;
    }

    private getAllAssetTagData(overViewCabinetData: any): void {
        const list: any = overViewCabinetData.Summary.Comunite;
        // 显示机柜
        this.display2DAsset(list);

        // 生成资产标签数据,结果已经是排序过的
        const assetTagData = this.createAssetTable(list);
        this.srcDataAssetTag = assetTagData;

        // 选中默认的资产标签
        this.selectAssetTag(assetTagData, this.selectedDevice);
    }

    private display2DAsset(assetArr: any): void {
        // 首先过滤出在位的资产标签
        this.actualAseetData.forEach((asset, index) => {
            // 挡板数据恢复默认。主要是为了避免 资产标签的热插拔操作对界面的影响
            asset.show = false;
            asset.uHeight = null;
            asset.name = '';
            asset.serialNumber = '';
            asset.color = '';
            asset.ledState = '';
            asset.initColor = '';
            asset.tips.content = this.translate.instant('IRM_OPEN_LIGHT');

            // 循环对比资产标签数据
            assetArr.forEach((item, indexAsset) => {
                const uHeight = item.UHeight ? item.UHeight : null;
                const duHeight = item.UHeight ? item.UHeight : 1;
                // 如果匹配到对应的资产标签数据
                if (item.TopUSlot === asset.uNum) {
                    // 背景图片是否显示
                    if (item.state !== 'Absent') {
                        asset.show = true;
                    }
                    asset.uHeight = uHeight;
                    asset.DuHeight = duHeight;
                    asset.name = item.Model;
                    asset.serialNumber = item.SerialNumber;
                    asset.color = item.IndicatorColor;
                    asset.ledState = item.IndicatorLED;
                    asset.tip = {
                        content: htmlEncode(item.Model),
                        position: 'right'
                    };
                    // 记录初始化时的颜色
                    asset.initColor = item.IndicatorColor;
                    asset.tips.content = (item.IndicatorLED === 'Blinking') ? this.translate.instant('IRM_CLOSE_LIGHT') :
                        this.translate.instant('IRM_OPEN_LIGHT');
                }
            });
        });
    }

    private createAssetTable(assetArr: any) {
        let temp: any = [];
        this.cabinetNum = {
            green: 0,
            yellow: 0,
            red: 0,
            grey: 0
        };
        assetArr.forEach((item) => {
            const uHeight = item.UHeight ? item.UHeight : 1;
            // 在位的情况
            if (item.state !== 'Absent') {
                const extendArr: any = [];
                for (const key in item.ExtendField) {
                    if (item.ExtendField.hasOwnProperty(key)) {
                        extendArr.push({
                            'key': key,
                            'value': item.ExtendField[key],
                            keyTip: {
                                content: htmlEncode(key),
                                position: 'left'
                            }
                        });
                    }
                }
                // 以下时间处理代码主要是为了解决IE下无法使用 - 的方式初始化时间而做的hack,处理逻辑，去除首尾空格，中间空格以T分隔
                const time = item.DiscoveredTime;
                let shelfTime = '';
                if (time) {
                    const arr = time.split(' ');
                    shelfTime = arr[0] + ' ' + arr[1];
                }
                temp.push({
                    state: item.state || CommonData.isEmpty,
                    // U位号
                    uNum: item.TopUSlot || CommonData.isEmpty,
                    // 标签UID
                    tagUid: item.RFIDTagUID || CommonData.isEmpty,
                    // 上架时间
                    'shelfTime': shelfTime || CommonData.isEmpty,
                    // 型号
                    pattern: item.Model || CommonData.isEmpty,
                    // 类别
                    type: item.DeviceType || CommonData.isEmpty,
                    // 序列号
                    serialNumber: item.SerialNumber || CommonData.isEmpty,
                    // 部件号
                    partNumber: item.PartNumber || CommonData.isEmpty,
                    // 占位U位数
                    'uHeight': uHeight || CommonData.isEmpty,
                    // 厂商
                    factory: item.Manufacturer || CommonData.isEmpty,
                    // 生命周期
                    lifeCircle: item.LifeCycleYear || CommonData.isEmpty,
                    // 开始使用时间
                    startTime: item.CheckInTime || CommonData.isEmpty,
                    // 重量
                    weight: item.WeightKg || CommonData.isEmpty,
                    // 功率
                    ratedPower: item.RatedPowerWatts || CommonData.isEmpty,
                    // 资产所有人
                    owner: item.AssetOwner || CommonData.isEmpty,
                    // 代表资产灯位颜色,取值有 Green  Yellow null
                    color: item.IndicatorColor || CommonData.isEmpty,
                    // 灯状态，亮：Lit  灭：Off 闪：Blinking
                    ledState: item.IndicatorLED || CommonData.isEmpty,
                    // 扩展字段,这里设计成数组的原因是由于自定义字段展示时，key值可以被编辑
                    extendField: extendArr,
                    // 是否只读  ReadWrite – 用户可读写	ReadOnly – 用户只读
                    RWCapability: item.RWCapability || CommonData.isEmpty
                });

                // 获取机柜概览状态数量
                if (item.state === 'Enabled' && item.health === 'OK') {
                    this.cabinetNum.green++;
                } else if (item.IndicatorColor === 'Yellow') {// 定位状态还是提过IndicatorColor统计
                    this.cabinetNum.yellow++;
                } else if (item.state === 'Enabled' && item.health === 'Warning') {
                    this.cabinetNum.red++;
                }
            } else {
                this.cabinetNum.grey++;
            }
            this.cabinetNum.grey = this.cabinetNum.grey - uHeight + 1;
        });
        // 传入 true ,则是逆序，false：升序
        temp = this.sortBy(temp, 'uNum', !this.topToBottom);
        return temp;
    }
    private selectAssetTag(assetTagData: any, singleAsset: any): void {
        if (!assetTagData.length) {
            return;
        }
        const defaultAsset = singleAsset.uNum > 0 ? singleAsset : assetTagData[0];

        // 挡板图片选中状态
        this.actualAseetData.forEach((item, indexAsset) => {
            if (item.uNum === defaultAsset.uNum) {
                item.selected = true;
                this.selectedDevice = item;
            }
        });

        // 选中对应的数据对象
        const self = this;
        assetTagData.forEach((item, indexAsset) => {
            if (item.uNum === defaultAsset.uNum) {
                self.currentDeviceTag = item;
                self.hasEditRole = (item.RWCapability === 'ReadWrite' || self.isRM110);
            }
        });
    }
    // 编辑资产
    private showTagDetail(): void {
        this.irmGlobalData.assetUid = this.selectedDevice.uNum;
        this.router.navigate(['/navigate/system/asset']);
    }
    private selectUbit(allData: any, curData: any): void {
        // 取消其他的选中状态
        allData.forEach((item) => {
            item.selected = false;
        });

        // 选中当前的数据
        curData.selected = true;
        const self = this;
        this.srcDataAssetTag.forEach((item, index) => {
            if (item.uNum === curData.uNum) {
                self.currentDeviceTag = item;
                self.hasEditRole = (item.RWCapability === 'ReadWrite' || self.isRM110);
            }
        });
        this.selectedDevice = curData;
    }
    private yellowLight(uData: any): void {
        if (!this.hasConfigureComponentsRight) {
            return;
        }
        // 查看是不是正在闪烁状态   Blinking  Lit  Off 闪/亮/灭
        const state = (uData.ledState === 'Blinking' && uData.color === 'Yellow');
        const openULight = this.translate.instant('IRM_OPEN_LIGHT');
        const closeULight = this.translate.instant('IRM_CLOSE_LIGHT');
        let param = {};
        if (state) {
            param = { 'IndicatorLED': 'Off' };
        } else {
            param = { 'IndicatorLED': 'Blinking', 'Duration': 50 };
        }

        uData.color = state ? uData.initColor : 'Yellow';
        uData.ledState = state ? (uData.initColor ? 'Lit' : 'Off') : 'Blinking';
        uData.tips.content = state ? openULight : closeULight;
    }

    // U位灯上的 mouseenter、mouseleave事件函数
    private tipUBitInfo(x: any): void {
        if (!this.hasConfigureComponentsRight) {
            return;
        }
        clearTimeout(this.timer);
        this.actualAseetData.forEach((item: any) => {
            item.tips.show = false;
        });
        this.timer = setTimeout(() => {
            x.tips.show = true;
        }, 200);
    }

    private closeTipInfo(x: any): void {
        if (!this.hasConfigureComponentsRight) {
            return;
        }
        clearTimeout(this.timer);
        x.tips.show = false;
    }
}
