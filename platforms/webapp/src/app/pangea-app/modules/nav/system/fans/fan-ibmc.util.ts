import { FanInfo } from './model/fanInfo';

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
        fanInfo = new FanInfo(fanName, name, frontFan.Model, reading, speedRatio, frontFan.PartNumber);
    } else {
        const fan = fans[0];
        const name = translate.instant('FAN_NAME') + fan.fanName.substr(3, fan.fanName.length - 1);
        const speedRatio = fan.SpeedRatio > 100 ? 100 : fan.SpeedRatio;
        fanInfo = new FanInfo(fan.fanName, name, fan.Model, fan.Speed, speedRatio, fan.PartNumber );
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
        if (Object.prototype.hasOwnProperty.call(fanInfos, fan)) {
            const fanInfo = fanStitching(fanInfos[fan], translate);
            fanInfoList.push(fanInfo);
        }
    }
    return fanInfoList;
}
