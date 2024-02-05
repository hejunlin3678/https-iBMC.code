export class Baffle {
    color: string;
    ledState: string;
    uNum: number;
    serialNumber: string;
    name: string;
    height: number;
    duHeight: any;
    uHeight: any;
    selected: boolean;
    show: boolean;
    group: number;
    separate: boolean;
    initColor: any;
    tips: any;
    constructor(uNum: number, height: number) {
        // 告警灯颜色
        this.color = '';
        // 灯是不是闪烁字符串
        this.ledState = '';
        // 对应的U位号
        this.uNum = uNum;
        // 对应的设备序列号
        this.serialNumber = '';
        // 绿灯资产条提示信息
        this.name = '';
        // 单个li的高度
        this.height = height;
        // 默认占1个U位
        this.duHeight = 1;
        this.uHeight = null;
        // 当前U位是否被选中
        this.selected = false;
        // 里面的设备图片是否显示
        this.show = false;
        // 分组 ， 每个资产条默认是占6个U位，也可能有其他的种类
        this.group = 0;
        // 是否是分组的边界
        this.separate = false;
        // 初始化的颜色
        this.initColor = null;
        this.tips = {
            triggerEvent: 'static',
            content: '',
            position: 'right',
            hasArrow: true,
            show: false,
            // U位是否是正处于定位状态（是不是闪烁，跟ledState有点重合,在代码中暂时没有用）
            state: false
        };
    }
}
