import { SDIExtendCard } from './extendCards/sdi-extend-card';
import { ExtendAttr } from '../../extend-attribute';
import { Ports } from '../ports';

export class SDICard extends ExtendAttr {
    // 名称
    private productName: [string, string];
    // 单板ID
    private boardId: [string, string];
    // PCB版本
    private pcbVersion: [string, string];
    // SODimm在位(DIMM00/DIMM10)
    private sODimm: [string, string];
    // M.2在位(M.2_0/M.2_1) || M.2在位(M.2_1/M.2_2) || M.2在位(M.1_0/M.2_0)
    private m2Device1Presence: [string, string];
    // 扩展卡在位数/总数
    private extendCardNum: [string, string];
    // 存储管理网口IP
    private storageManageIP: [string, string];
    // VLAN ID
    private vlanId: [string, string];
    // 系统启动方式
    private bootOption: [string, string];
    // 扩展卡信息集合
    private extendCards: SDIExtendCard[];
    // 端口属性
    private ports: Ports;

    constructor(
        productName: string,
        boardId: string,
        pcbVersion: string,
        storageManageIP: string,
        vlanId: string,
        bootOption: string,
        ports: Ports
    ) {
        super();
        if (productName !== '--') {
            this.productName = ['COMMON_NAME', productName];
        }
        if (boardId !== '--') {
            this.boardId = ['OTHER_BOARD_ID', boardId];
        }
        if (pcbVersion !== '--') {
            this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        }
        this.m2Device1Presence = [null, null];
        if (storageManageIP !== '--') {
            this.storageManageIP = ['OTHER_PORT_IP', storageManageIP];
        }
        this.vlanId = ['VLAN ID', vlanId];
        if (bootOption !== '--') {
            this.bootOption = ['OTHER_SYSMETHOD', bootOption];
        }
        this.extendCards = [];
        this.ports = ports;
    }

    set setExtendCards(extendCards: SDIExtendCard[]) {
        this.extendCards = extendCards;
    }

    set setSODimm(sODimm: string) {
        this.sODimm = ['OTHER_SODINPOSITION', sODimm];
    }

    set setExtendCardNum(cardNum: string) {
        this.extendCardNum = ['OTHER_CARD_INPOSITION', cardNum];
    }

    set setM2Device1Presence(m2Device: [string, string]) {
        this.m2Device1Presence = m2Device;
    }

    addExtendCards(extendCard: SDIExtendCard): void {
        this.extendCards.push(extendCard);
    }
}
