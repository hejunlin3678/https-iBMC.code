#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
# script for install Apache Maven 3.8.8
# version: 3.8.8
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
# check install files
ls apache-maven-3.8.8-bin.tar.gz

# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}
tar xzf apache-maven-3.8.8-bin.tar.gz
cp -fr apache-maven-3.8.8/* ${install_dir}

#set setting
cat >"${install_dir}"/conf/settings.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>

<settings xmlns="https://maven.apache.org/SETTINGS/1.0.0"
          xmlns:xsi="https://www.w3.org/2001/XMLSchema-instance"
          xsi:schemaLocation="https://maven.apache.org/SETTINGS/1.0.0 https://maven.apache.org/xsd/settings-1.0.0.xsd">

    <localRepository>/usr1/jenkins/workspace/compile/ibmc_cache/MVN_HOME</localRepository>
    
    <mirrors>
        <mirror>
            <id>central</id>
            <mirrorOf>*,!3maven</mirrorOf>
            <url>https://cmc.centralrepo.rnd.huawei.com/artifactory/maven-central-repo/</url>
        </mirror>
    </mirrors>
    <profiles>
        <profile>
            <id>artifactory</id>
            <pluginRepositories>
                <pluginRepository>
                    <id>3maven</id>
                    <name>opensource</name>
                    <url>https://cmc.centralrepo.rnd.huawei.com/artifactory/product_maven/</url>
                    <releases><enabled>true</enabled></releases>
                    <snapshots><enabled>false</enabled></snapshots>
                </pluginRepository>
            </pluginRepositories>
        </profile>
    </profiles>

    <activeProfiles>
        <activeProfile>artifactory</activeProfile>
    </activeProfiles>
</settings>
EOF

#set env
sed -i '/MAVEN_HOME/d' /etc/profile
echo "export MAVEN_HOME=${install_dir}" >> /etc/profile
echo 'export CLASSPATH=$MAVEN_HOME/lib:$CLASSPATH' >> /etc/profile
echo 'export PATH=$MAVEN_HOME/bin:$PATH' >> /etc/profile

#set link
ln -sf ${install_dir}/bin/mvn /usr/local/bin/mvn
ln -sf ${install_dir}/bin/mvnDebug /usr/local/bin/mvnDebug
ln -sf ${install_dir}/bin/mvnyjp /usr/local/bin/mvnyjp


chmod -R 755 ${install_dir}