// E9000 前视图 和 后视图 组件
import { Component, Input, OnInit } from '@angular/core';

@Component({
    selector: 'e9000-frontAndRearView',
    templateUrl: './e9000-frontAndRearView.component.html',
    styleUrls: ['./e9000-frontAndRearView.component.scss', './e9000-frontView.scss'],
})
export class E9000FrontAndRearViewComponent implements OnInit {
    // 产品数据
    @Input() productData = {};

    public swiSeries = ['1E', '2X', '3X', '4E'];

    // blade节点 PSU电源  fan风扇 swi交换板 mms管理板

    // 前视图数据 : FrontInfo[]
    public frontList = [];

    // 电源数据 psu1-psu3
    public psuList1 = [];

    // 电源数据 psu4-psu6
    public psuList2 = [];

    // 风扇数据 fan1-fan6
    public fansList1 = [];

    // 风扇数据 fan13-fan14
    public fansList2 = [];

    // 风扇数据 fan7-fan12
    public fansList3 = [];

    // 管理板数据 mm
    public mmList = [];

    // 交换板数据 swi
    public swiList = [];

    // PSU 电源数据
    initPsuData(data) {
        if (data ?.Power ?.SupplyList) {
            const supplyList = [...data.Power.SupplyList];

            supplyList.forEach((item) => {
                const num = item.Name.substr(3) * 1;
                if (1 <= num && num <= 3) {
                    this.psuList1[3 - num].health = item.Health;
                } else if (3 <= num && num <= 6) {
                    this.psuList2[6 - num].health = item.Health;
                }
            });
        }
    }

    // Fan 风扇数据
    initFanData(data) {
        if (data ?.Fan) {
            const fan = [...data.Fan];
            fan.forEach((item) => {
                const num = item.Name.substr(3) * 1;
                if (1 <= num && num <= 6) {
                    this.fansList1[6 - num].health = item.Health;
                } else if (7 <= num && num <= 12) {
                    this.fansList3[12 - num].health = item.Health;
                } else if (13 <= num && num <= 14) {
                    this.fansList2[14 - num].health = item.Health;
                }
            });
        }
    }

    // 下面俩个方法是当通信丢失给默认宽高
    createArray(arr) {
        const newArr = [];
        const len = arr.length;
        for (const item of arr) {
            if (item.abnormal) {
                newArr.push(item);
            }
        }
        const requireArr = this.filterArray(newArr, arr);
        return requireArr;
    }
    filterArray(newArr, arr) {
        for (const item of newArr) {
            if (item.index > 16) {
                // 判断异常对应的卡槽是否在位
                if (arr[item.index - 16 - 1].classType === 'Disabled') {
                    arr[item.index - 1].classType = 'half';
                } else {
                    arr[item.index - 1].classType = 'quarter';
                    arr[item.index - 16 - 1].classType = 'quarter';
                }
            } else {
                if (arr[item.index + 16 - 1].classType === 'Disabled') {
                    arr[item.index - 1].classType = 'half';
                } else {
                    arr[item.index - 1].classType = 'quarter';
                    arr[item.index + 16 - 1].classType = 'quarter';
                }
            }
        }
        return arr;
    }


    // blade或者slot-前视图-刀片数据   Swi-后视图-交换板数据   HMM-后视图-管理板数据
    init_slot_swi_mm_data(data) {
        if (data ?.Component) {
            const frontViewData = data.Component;

            for (const item of frontViewData) {
                // 前视图刀片
                if (
                    item.Id.indexOf('Blade') >= 0 &&
                    item.Id !== 'Blade37' &&
                    item.Id !== 'Blade38'
                ) {
                    const frontView = {
                        id: item.Id,
                        classType: 'Disabled',
                        index: 0,
                        abnormal: false,
                        powerState: '',
                        health: item.Health
                    };

                    if (item.Width * 1 === 2 && item.Height * 1 === 2) {
                        frontView.classType = 'full';
                    } else if (item.Width * 1 === 1 && item.Height * 1 === 2) {
                        frontView.classType = 'half';
                    } else if (item.Width * 1 === 1 && item.Height * 1 === 1) {
                        frontView.classType = 'quarter';
                    }

                    this.initPowerState(item, frontView);

                    if (item.HotswapState === 7 || item.HotswapState === 0) {
                        if (item.Width == null && item.Height == null) {
                            frontView.index = parseInt(item.Id.substr(5), 10);
                            frontView.abnormal = true;
                        }
                    }

                    this.frontList[item.Id.substr(5) - 1] = frontView;
                }

                // 后视图交换板 swi
                if (item.Id.indexOf('Swi') >= 0) {
                    const arrSWI = {
                        name: '',
                        tagName: '',
                        health: item.Health
                    };

                    arrSWI.name = item.HotswapState === 0 ? '' : item.Id;
                    arrSWI.tagName = this.swiSeries[item.Id.substr(3) - 1];
                    this.swiList[item.Id.substr(3) - 1] = arrSWI;
                }

                // HMM 主备板,添加了blade37,blade38
                if (
                    item.Id.indexOf('HMM') >= 0 ||
                    item.Id === 'Blade37' ||
                    item.Id === 'Blade38'
                ) {
                    // HMM
                    if (item.Id.indexOf('HMM') >= 0) {
                        if (item.Id === 'HMM1') {
                            this.mmList[1].health = item.Health;
                            this.mmList[1].ative = '_ative ' + item.Health;
                        }
                        if (item.Id === 'HMM2') {
                            this.mmList[0].health = item.Health;
                            this.mmList[0].ative = '_ative ' + item.Health;
                        }
                    }

                    // Blade37
                    if (item.Id === 'Blade37') {
                        this.mmList[1].slot.health = item.Health;

                        this.initPowerState(item, this.mmList[1].slot);
                    }

                    // Blade38
                    if (item.Id === 'Blade38') {
                        this.mmList[0].slot.health = item.Health;

                        this.initPowerState(item, this.mmList[0].slot);
                    }
                }
            }

            this.frontList = this.createArray(this.frontList);
        }
    }

    // 针对powerState的值是什么的判断进行封装
    initPowerState(item, aimItem) {

        if (item.HotswapState === 7 || item.HotswapState === 0) {
            aimItem.powerState = 'notConnected';
        } else if (item.ManagedByMM === false) {
            aimItem.powerState = 'versionLow';
        } else {
            const state = item.HotswapState;
            switch (state) {
                case 1:
                    aimItem.powerState = 'Off';
                    break;
                case 4:
                    aimItem.powerState = 'On';
                    break;
            }
        }
    }

    // 初始化数据
    initData() {
        // 前视图数据
        this.frontList = [];
        for (let i = 1; i <= 32; i++) {
            this.frontList.push({
                id: 'Blade' + i,
                classType: 'Disabled',
                index: 0,
                abnormal: false,
                powerState: '',
                health: ''
            });
        }

        // Swi 管理板数据
        this.swiList = [];
        for (let i = 1; i <= 4; i++) {
            this.swiList.push({
                name: 'Swi' + i,
                ative: '',
                type: 'Swi',
                state: 'Enabled',
                tagName: this.swiSeries[i - 1]
            });
        }

        // MM/HMM 主备板数据
        this.mmList = [
            {
                name: 'MM2',
                ative: '',
                type: 'HMM',
                state: 'Enabled',
                health: '',
                powerState: '',
                slot: {
                    name: 'Blade38',
                    ative: '',
                    type: 'HMM',
                    state: 'Enabled',
                    health: '',
                    powerState: ''
                }
            },
            {
                name: 'MM1',
                ative: '',
                type: 'HMM',
                state: 'Enabled',
                health: '',
                powerState: '',
                slot: {
                    name: 'Blade37',
                    ative: '',
                    type: 'HMM',
                    state: 'Enabled',
                    health: '',
                    powerState: ''
                }
            }
        ];

        // Fans 风扇数据 fan1-fan6
        for (let i = 6; i >= 1; i--) {
            this.fansList1.push({
                name: 'Fan' + i,
                health: ''
            });
        }

        // Fans 风扇数据 fan7-fan12
        for (let i = 12; i >= 7; i--) {
            this.fansList3.push({
                name: 'Fan' + i,
                health: ''
            });
        }

        // Fans 风扇数据 fan3-fan14
        for (let i = 13; i <= 14; i++) {
            this.fansList2.push({
                name: 'Fan' + i,
                health: ''
            });
        }

        // PSU 电源数据 PSU1-3
        for (let i = 3; i >= 1; i--) {
            this.psuList1.push({
                health: '',
                name: 'PSU' + i
            });
        }

        // PSU 电源数据 PSU6-4
        for (let i = 6; i >= 4; i--) {
            this.psuList2.push({
                health: '',
                name: 'PSU' + i
            });
        }
    }

    constructor() {
        // 初始化数据
        this.initData();
    }

    ngOnInit(): void {
        // PSU 电源数据
        this.initPsuData(this.productData);

        // Fan 风扇数据
        this.initFanData(this.productData);

        // blade或者slot-前视图刀片   Swi-后视图交换板数据   HMM-后视图管理板数据
        this.init_slot_swi_mm_data(this.productData);
    }
}
