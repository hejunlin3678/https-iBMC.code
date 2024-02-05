import { Component, OnInit } from '@angular/core';
import { NetService } from '../../net.service';
import { NetCard } from '../../classes';
import { NetCardService } from './net-card.service';
import { NetPortArr } from '../../classes/ports/net/net-port-Arr';
import { TiTableSrcData, TiTableRowData, TiMessageService } from '@cloud/tiny3';
import { LoadingService, HttpService, AlertMessageService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { NetworkService } from 'src/app/common-app/modules/manager/network';
import { GlobalDataService, PRODUCTTYPE } from 'src/app/common-app/service';

@Component({
    selector: 'app-net-card',
    templateUrl: './net-card.component.html',
    styleUrls: ['./net-card.component.scss']
})
export class NetCardComponent implements OnInit {

    public netCard: NetCard;
    public netPort: NetPortArr;
    private activeNum: number;
    private activeIndex: number;
    public hotPlugSupported: boolean;
    public hotPlugAttention: boolean;
    public hotPlugCtrlStatus: string;
    public adapterId: string;
    public ocpRadioTip: string;
    public srcData: TiTableSrcData;
    public columns;
    public data: TiTableRowData[] = [];
    public type: string = 'net';
    public ocpPort: boolean;

    constructor(
        private netService: NetService,
        private netCardService: NetCardService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private http: HttpService,
        private alert: AlertMessageService,
        private loading: LoadingService,
        private networkService: NetworkService,
        private globalData: GlobalDataService
    ) { }

    ngOnInit() {
        this.srcData = {
            data: this.data,
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
        this.activeNum = this.netService.activeNum;
        this.activeIndex = this.netService.activeIndex;
        this.netCard = this.netService.netTree.getCardArray[this.activeIndex].cards[this.activeNum] as NetCard;
        if (!this.netCard.getInit) {
            this.loading.state.next(true);
            this.netCardService.factory(this.netCard.getMoreInfo, this.netCard.getName).subscribe(
                (card) => {
                    this.netCard = card;
                    this.netPort = this.netCard.getPort;
                    this.netCard.hotPlugCtrlStatus = card.hotPlugCtrlStatus;
                    this.netCard.hotPlugSupported = card.hotPlugSupported;
                    this.netCard.adapterId = card.adapterId;
                    this.netCard.hotPlugAttention = card.hotPlugAttention;
                    this.columns = this.netCard.getPort ? this.netCard.getPort.titles : [];
                    this.srcData.data = this.netCard.getPort ? this.netCard.getPort.netPorts : [];
                    this.netService.netTree.getCardArray[this.activeIndex].cards[this.activeNum] = card;
                    this.loading.state.next(false);
                }
            );
        } else {
            this.columns = this.netCard.getPort ? this.netCard.getPort.titles : [];
            this.srcData.data = this.netCard.getPort ? this.netCard.getPort.netPorts : [];
        }
        this.hotPlugCtrlStatus = this.netCard.hotPlugCtrlStatus;
        this.hotPlugSupported = this.netCard.hotPlugSupported;
        this.adapterId = this.netCard.adapterId;
        this.hotPlugAttention = this.netCard.hotPlugAttention;
        if (this.hotPlugCtrlStatus === 'Insertable') {
            this.ocpRadioTip = this.translate.instant('OCPRADIO_INSERT');
        } else if (this.hotPlugCtrlStatus === 'Removable') {
            this.ocpRadioTip = this.translate.instant('OCPRADIO_UNPLUGS');
        }
    }

    public sliderClick(portName) {
        let currentData = null;
        this.netCard.getPort.netPorts.forEach((item, index) => {
            if (item['portName'] + index === portName) {
                currentData = item;
                return;
            }
        });
        if (!currentData.extendAttr.init) {
            this.netCardService.extendAttrFactory(currentData, 'Mbps').subscribe((value) => {
                currentData.extendAttr = value;
            });
        }
    }

    public ocpClick() {
        this.networkService.networkList().subscribe((res) => {
            const networkPort = res[0].networkPortType.Type;
            if (networkPort.indexOf('OCP') !== -1) {
                this.ocpPort = true;
            } else {
                this.ocpPort = false;
            }
            const instance = this.tiMessage.open({
                id: 'oneKeyModel',
                type: 'prompt',
                okButton: {
                    show: true,
                    autofocus: false,
                    click: () => {
                        let param = {};
                        if (this.hotPlugCtrlStatus === 'Insertable') {
                            param = {
                                HotPlugAttention: false
                            };
                        } else if (this.hotPlugCtrlStatus === 'Removable') {
                            param = {
                                HotPlugAttention: true
                            };
                        }
                        this.http.patch(`/UI/Rest/System/NetworkAdapter/${this.adapterId}`, param).subscribe({
                            next: () => {
                                if (this.globalData.productType === PRODUCTTYPE.TIANCHI) {
                                    this.hotPlugCtrlStatus = 'Inoperable';
                                }
                                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                            },
                            error: (error) => {
                            }
                        });
                        instance.close();
                    }
                },
                cancelButton: {
                    show: true
                },
                closeIcon: false,
                title: this.hotPlugCtrlStatus === 'Removable' ? this.translate.instant('UNPLUGS_TITLE')
                    : this.translate.instant('INSERT_TITLE'),
                content: this.hotPlugCtrlStatus === 'Insertable' ? this.translate.instant('INSERT_CONTENT')
                    : this.ocpPort ? this.translate.instant('UNPLUGS_ONOCP_CONTENT') : this.translate.instant('UNPLUGS_CONTENT')
            });
        });
    }
}
