import { CommonData } from '../models/common.model';
import { FanInfo } from '../models/fanInfo';

// 风扇数据处理
function fanStitching(fans, translate) {
    let fanInfo: FanInfo = null;
    // 双风扇处理
    if (fans.length === 2) {
        let frontFan = null;
        let rearFan = null;
        if (fans[0].Name.indexOf('Front') > -1) {
            frontFan = fans[0];
            rearFan = fans[1];
        } else {
            frontFan = fans[1];
            rearFan = fans[0];
        }
        const fanName = frontFan.fanName;
        const name = translate.instant('FAN_NAME') + fanName.substr(3, fanName.length - 1) + ' ' + translate.instant('FAN_LOCATION_NAME');
        const frontReading = frontFan.Speed === null ? '--' : frontFan.Speed;
        const rearReading = rearFan.Speed === null ? '--' : rearFan.Speed;
        const reading = frontReading + '/' + rearReading;
        const frontRatio = frontFan.SpeedRatio === null ? '--' : (frontFan.SpeedRatio > 100 ? 100 : frontFan.SpeedRatio);
        const rearRatio = rearFan.SpeedRatio === null ? '--' : (rearFan.SpeedRatio > 100 ? 100 : rearFan.SpeedRatio);
        const speedRatio = frontRatio + '/' + rearRatio;
        const presence = frontFan?.Presence === 0 ? translate.instant('POWER_NOT_DETECTED') : '' ;
        fanInfo = presence ? new FanInfo(fanName, name, null, null, null, null, presence)
                    : new FanInfo(fanName, name, frontFan.Model, reading, speedRatio, frontFan.PartNumber, presence);
    } else {
        const fan = fans[0];
        const name = translate.instant('FAN_NAME') + fan.fanName.substr(3, fan.fanName.length - 1);
        const speedRatio = fan.SpeedRatio > 100 ? 100 : fan.SpeedRatio;
        const presence = fan?.Presence === 0 ? translate.instant('POWER_NOT_DETECTED') : '' ;
        // Presence为0时代表风扇未检测到,风扇的数据信息均显示'--'
        fanInfo = presence ? new FanInfo(fan.fanName, name, null, null, null, null, presence)
                    : new FanInfo(fan.fanName, name, fan.Model, fan.Speed, speedRatio, fan.PartNumber, null);
    }
    return fanInfo;
}


export function getFanInfo(data, translate) {
    const fanInfoList: FanInfo[] = [];
    const fans = data.Fans;
    const fanInfos = {};
    // 遍历数组，将相同风扇位的内容放到一个以该Name命名的数组中
    fans.forEach(element => {
        let isExist = false;
        const name = element.Name.split(' ')[0];
        element['fanName'] = name;
        if (fanInfos[name]) {
            fanInfos[name].push(element);
            isExist = true;
        }
        if (!isExist) {
            fanInfos[name] = [element];
        }
    });
    for (const fan in fanInfos) {
        if (fanInfos.hasOwnProperty(fan)) {
            const fanInfo = fanStitching(fanInfos[fan], translate);
            fanInfoList.push(fanInfo);
        }
    }
    return fanInfoList;
}

export function hideFansAndPower() {

    /*
    * 隐藏风扇和电源功率
    * 当前按产品名隐藏,以下产品隐藏
    * 'CST0210V6', 'CST1020V6', 'CST1220V6'
    */
    const productName = localStorage.getItem('productName');
    const hideFansAndPowerList = CommonData.hideFansAndPowerList;
    const hideFansAndPowerStata: boolean = hideFansAndPowerList.includes(productName);
    return hideFansAndPowerStata;
}
