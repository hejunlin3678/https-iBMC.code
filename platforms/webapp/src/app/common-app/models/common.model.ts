import { IMainMenuItem } from './common.interface';

export class CommonData {

    public static readonly isEmpty: string = '--';

    public static readonly fontFamily: string = 'PingFang, Hiragino Sans GB, Microsoft YaHei, Microsoft JhengHei, Huawei sans, Arial, Roboto, Noto sans';

    public static readonly LANG_ARR_NAME = {
        'en': 'English',
        'zh': '中文',
        'ja': '日本語',
        'fr': 'Français',
        'ru': 'Русский'
    };

    public static readonly LANG_OPTIONS: any[] = [
		{ id: 'en-US', label: CommonData.LANG_ARR_NAME.en, langSet: 'en', hidden: false, tiny3Set: 'en-US' },
		{ id: 'zh-CN', label: CommonData.LANG_ARR_NAME.zh, langSet: 'zh', hidden: false, tiny3Set: 'zh-CN' },
		{ id: 'ja-JA', label: CommonData.LANG_ARR_NAME.ja, langSet: 'ja', hidden: true, tiny3Set: 'ja-JA' },
		{ id: 'fr-FR', label: CommonData.LANG_ARR_NAME.fr, langSet: 'fr', hidden: true, tiny3Set: 'fr-FR' },
		{ id: 'ru-RU', label: CommonData.LANG_ARR_NAME.ru, langSet: 'ru', hidden: true, tiny3Set: 'ru-RU' }
    ];

    public static readonly BROWSER_VERSION_SUPPORT = {
        firefox: 63,
        chrome: 70,
        safari: 11,
        ie: 11
    };

    public static headerLogSrc: string;
    public static mainMenu: IMainMenuItem[];
    public static routeConfig;
    public static productType: number;
    public static hideFansAndPowerList = ['CST0210V6', 'CST1020V6', 'CST1220V6', 'CST0220V6'];

    private constructor() {}
}
