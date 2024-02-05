import { Component, OnInit } from '@angular/core';

import { HomeService } from '../../services';

@Component({
  selector: 'app-home-virtual-modal',
  templateUrl: './home-virtual-modal.component.html',
  styleUrls: ['./home-virtual-modal.component.scss'],
})
export class HomeVirtualModalComponent implements OnInit {

  public downloadKVMLink = this.homeService.downloadKVMLink;

  public errorMessageHelp = [
    {
      id: 1,
      label: 'REMOTE_ERROR_ITEM1',
      helpURL: '/bmc_help_0048.html'
    },
    {
      id: 2,
      label: 'REMOTE_ERROR_ITEM2',
      helpURL: '/bmc_help_0052.html'
    },
    {
      id: 3,
      label: 'REMOTE_ERROR_ITEM3',
      helpURL: '/bmc_help_0063.html'
    },
    {
      id: 4,
      label: 'REMOTE_ERROR_ITEM4',
      helpURL: '/bmc_help_0064.html'
    }
  ];

  constructor(private homeService: HomeService) { }

  ngOnInit() {
  }

  public goHelpPage(helpUrl: string) {
    const language: string = localStorage.getItem('locale');
    let localeLanguage: string;
    switch (language) {
      case 'zh-CN':
        localeLanguage = 'zh-cn';
        break;
      case 'en-US':
        localeLanguage = 'en-us';
        break;
      case 'ja-JA':
        localeLanguage = 'jap-ja';
        break;
      case 'fr-FR':
        localeLanguage = 'fre-fr';
        break;
      case 'ru-RU':
        localeLanguage = 'ru-ru';
        break;
      default:
        localeLanguage = 'zh-cn';
    }
    const url = `/help/${localeLanguage}${helpUrl}`;
    window.open(url, '_blank');
  }

  public close() {

  }

}
