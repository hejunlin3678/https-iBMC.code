import { Injectable } from '@angular/core';
import { CommonData } from 'src/app/common-app/models';
import { PRODUCT } from './product.type';

@Injectable()

export class HelpRelationService {
    constructor() { }
    public getHelpHtml;
    public getBmcHelpHtml = {
        'login': 'bmc_help_0005.html',
        'home': 'bmc_help_0006.html',
        'product': 'bmc_help_0056.html',
        'processor': 'bmc_help_0057.html',
        'memory': 'bmc_help_0058.html',
        'net': 'bmc_help_0059.html',
        'sensor': 'bmc_help_0060.html',
        'units': 'bmc_help_0069.html',
        'others': 'bmc_help_0061.html',
        'monitor': 'bmc_help_0009.html',
        'storage': 'bmc_help_0010.html',
        'power': 'bmc_help_0011.html',
        'fans': 'bmc_help_0012.html',
        'bios': 'bmc_help_0013.html',
        'partity': 'bmc_help_0014.html',
        'alarm': 'bmc_help_0016.html',
        'syslog': 'bmc_help_0017.html',
        'email': 'bmc_help_0017.html',
        'trap': 'bmc_help_0017.html',
        'fdm': 'bmc_help_0018.html',
        'video': 'bmc_help_0019.html',
        'systemlog': 'bmc_help_0020.html',
        'ibmclog': 'bmc_help_0021.html',
        'worknote': 'bmc_help_0022.html',
        'localuser': 'bmc_help_0024.html',
        'ldap': 'bmc_help_0025.html',
        'kerberos': 'bmc_help_0026.html',
        'root': 'bmc_help_0027.html',
        'client': 'bmc_help_0027.html',
        'online-user': 'bmc_help_0028.html',
        'security': 'bmc_help_0029.html',
        'port': 'bmc_help_0031.html',
        'web': 'bmc_help_0032.html',
        'kvm': 'bmc_help_0033.html',
        'vmm': 'bmc_help_0034.html',
        'vnc': 'bmc_help_0035.html',
        'snmp': 'bmc_help_0036.html',
        'network': 'bmc_help_0038.html',
        'ntp': 'bmc_help_0039.html',
        'upgrade': 'bmc_help_0040.html',
        'configupgrade': 'bmc_help_0041.html',
        'language': 'bmc_help_0042.html',
        'license': 'bmc_help_0043.html',
        'ibma': 'bmc_help_0065.html',
        'sp': 'bmc_help_0068.html',
        'usb': 'bmc_help_0067.html',
        'cert-update': 'bmc_help_0071.html',
        'cert-update-ssl': 'bmc_help_0073.html',
        'cert-update-bios': 'bmc_help_0074.html',
        'tpcm': 'bmc_help_0075.html',
    };

    public getIrmHelpHtml = {
        'login': 'irm_help_0004.html',
        'home': 'irm_help_0005.html',
        'product': 'irm_help_0009.html',
        'sensor': 'irm_help_0009.html',
        'others': 'irm_help_0009.html',
        'power': 'irm_help_0027.html',
        'battery': 'irm_help_0028.html',
        'fans': 'irm_help_0029.html',
        'asset': 'irm_help_0030.html',
        'alarm': 'irm_help_0007.html',
        'syslog': 'irm_help_0010.html',
        'email': 'irm_help_0010.html',
        'trap': 'irm_help_0010.html',
        'ibmclog': 'irm_help_0011.html',
        'localuser': 'irm_help_0013.html',
        'ldap': 'irm_help_0014.html',
        'kerberos': 'irm_help_0031.html',
        'online-user': 'irm_help_0015.html',
        'security': 'irm_help_0016.html',
        'port': 'irm_help_0025.html',
        'web': 'irm_help_0021.html',
        'snmp': 'irm_help_0023.html',
        'network': 'irm_help_0018.html',
        'ntp': 'irm_help_0019.html',
        'upgrade': 'irm_help_0024.html',
        'configupgrade': 'irm_help_0041.html',
        'language': 'irm_help_0026.html',
        'cert-update': 'irm_help_0071.html'
    };

    public getEmmHelpHtml = {
        'login': 'it_emm_0049.html',
        'home': 'it_emm_0003.html',
        'power': 'it_emm_0005.html',
        'fans': 'it_emm_0009.html',
        'alarm': 'it_emm_0015.html',
        'syslog': 'it_emm_0016.html',
        'email': 'it_emm_0016.html',
        'ibmclog': 'it_emm_0017.html',
        'localuser': 'it_emm_0021.html',
        'ldap': 'it_emm_0022.html',
        'online-user': 'it_emm_0043.html',
        'security': 'it_emm_0023.html',
        'port': 'it_emm_0040.html',
        'web': 'it_emm_0028.html',
        'network': 'it_emm_0033.html',
        'ntp': 'it_emm_0034.html',
        'upgrade': 'it_emm_0038.html',
        'language': 'it_emm_0035.html',
        'bmcUser': 'it_emm_0011.html',
        'bmcNetwork': 'it_emm_0012.html',
        'powerOnOff': 'it_emm_0013.html',
        'restSwi': 'it_emm_0047.html',
        'alarm-config': 'it_emm_0018.html',
        'worknote': 'it_emm_0048.html',
        'root': 'it_emm_0042.html',
        'switch': 'it_emm_0036.html'
    };

    public getHtml(res) {
        switch (res) {
            case PRODUCT.IBMC:
                this.getHelpHtml = this.getBmcHelpHtml;
                break;
            case PRODUCT.IRM:
                this.getHelpHtml = this.getIrmHelpHtml;
                break;
            case PRODUCT.PANGEA:
                this.getHelpHtml = this.getBmcHelpHtml;
                break;
            case PRODUCT.EMM:
                this.getHelpHtml = this.getEmmHelpHtml;
                break;
        }
    }

    public helpRelation(route) {
        this.getHtml(CommonData.productType);
        const router = route.split('?')[0];
        for (const key in this.getHelpHtml) {
            if (router === key) {
                return this.getHelpHtml[key];
            }
        }
    }

}

