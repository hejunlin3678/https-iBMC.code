// TCE8080 前视图 和 后视图 组件
import { Component, Input, OnInit } from '@angular/core';


@Component({
    selector: 'tce8080-frontAndRearView',
    templateUrl: './tce8080-frontAndRearView.component.html',
    styleUrls: ['./tce8080-frontAndRearView.component.scss'],
})
export class Tce8080FrontAndRearViewComponent implements OnInit {
    // 产品数据
    @Input() productData = {};

    // blade节点 PSU电源  fan风扇 swi交换板 mms管理板

    // 前视图数据 : FrontInfo[]
    public frontList = [];

    // 电源数据 psu
    public psuList = [];

    // 风扇数据 第1行
    public fansList1 = [];

    // 风扇数据 第2行
    public fansList2 = [];

    // 风扇数据 第3行
    public fansList3 = [];

    // 风扇数据 第4行
    public fansList4 = [];

    // 管理板数据 mm
    public mmList = [];

    // PSU 电源数据
    initPsuData(data) {
        if (data ?.Power ?.SupplyList) {
            const supplyList = [...data.Power.SupplyList];

            supplyList.forEach((item) => {
                const num = item.Name.substr(3) * 1;
                if (num <= 8) {
                    this.psuList[8 - num].health = item.Health;
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
                if (num <= 16) {
                    if (num % 4 === 0) {
                        this.fansList1[4 - num / 4].health = item.Health;
                    } else if (num % 4 === 3) {
                        this.fansList2[3 - (num - 3) / 4].health = item.Health;
                    } else if (num % 4 === 2) {
                        this.fansList3[3 - (num - 2) / 4].health = item.Health;
                    } else {
                        this.fansList4[3 - (num - 1) / 4].health = item.Health;
                    }
                }
            });
        }
    }

    // blade或者slot-前视图-刀片数据   mm-后视图-管理板数据
    init_slot_swi_data(data) {
        if (data ?.Component) {
            const frontViewData = data.Component;

            for (const item of frontViewData) {
                // 前视图刀片
                const num = item.Id.substr(5) * 1;
                if (item.Id.indexOf('Blade') >= 0 && num < 9) {
                    const frontView = {
                        id: item.Id,
                        powerState: '',
                        health: item.Health
                    };

                    // 加工得到powerState连接状态的数据
                    this.initPowerState(item, frontView);

                    this.frontList[8 - num] = frontView;
                }

                // HMM 主备板
                if (item.Id.indexOf('HMM') >= 0) {
                    if (item.Id === 'HMM1') {
                        this.mmList[1].health = item.Health;
                    }
                    if (item.Id === 'HMM2') {
                        this.mmList[0].health = item.Health;
                    }
                }
            }
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
        for (let i = 8; i >= 1; i--) {
            this.frontList.push({
                id: 'Blade' + i,
                powerState: '',
                health: ''
            });
        }

        // MM/HMM 主备板数据
        this.mmList = [
            {
                name: 'SMM2',
                health: '',
            },
            {
                name: 'SMM1',
                health: '',
            }
        ];


        // Fans 风扇数据
        for (let i = 4; i >= 1; i--) {
            this.fansList1.push({
                name: 'Fan' + 4 * i,
                health: ''
            });

            this.fansList2.push({
                name: 'Fan' + (4 * i - 1),
                health: ''
            });

            this.fansList3.push({
                name: 'Fan' + (4 * i - 2),
                health: ''
            });

            this.fansList4.push({
                name: 'Fan' + (4 * i - 3),
                health: ''
            });
        }

        // PSU 电源数据
        for (let i = 8; i >= 1; i--) {
            this.psuList.push({
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

        // blade或者slot-前视图刀片   Swi-后视图交换板数据
        this.init_slot_swi_data(this.productData);
    }
}
