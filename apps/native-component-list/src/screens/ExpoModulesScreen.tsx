import React, { useCallback } from 'react';
import { ScrollView, StyleSheet, View } from 'react-native';

import Button from '../components/Button';
import HeadingText from '../components/HeadingText';
import MonoText from '../components/MonoText';

// Custom JSON replacer that can stringify functions.
const customJsonReplacer = (_: string, value: any) => {
  return typeof value === 'function' ? value.toString().replace(/\s+/g, ' ') : value;
};

export default class ExpoModulesScreen extends React.PureComponent<any, any> {
  render() {
    // const modules = { ...globalThis.expo?.modules };
    // const moduleNames = Object.keys(modules);

    return (
      <ScrollView style={styles.scrollView}>
        <HeadingText>Host object is installed</HeadingText>
        <MonoText>{`'modules' in globalThis.expo => ${'modules' in globalThis.expo!}`}</MonoText>

        <Test />

        {/* <HeadingText>Available Expo modules</HeadingText>
        <MonoText>
          {`Object.keys(global.expo.modules) => [\n  ${moduleNames.join(',\n  ')}\n]`}
        </MonoText>

        {moduleNames.map((moduleName) => {
          return (
            <View key={moduleName}>
              <HeadingText>Module: {moduleName}</HeadingText>
              <MonoText>{JSON.stringify(modules[moduleName], customJsonReplacer, 2)}</MonoText>
            </View>
          );
        })} */}
      </ScrollView>
    );
  }
}

function Test() {
  const runTest = useCallback(() => {
    const VideoPlayer = globalThis.expo!.modules!.ExpoVideo.VideoPlayer;
    const startTime = performance.now();

    for (let i = 0; i < 10_000; i++) {
      const player = new VideoPlayer('');

      expo.SharedObject.release(player);
    }

    console.log('done in', performance.now() - startTime);
  }, []);

  return (
    <View>
      <Button title="test" onPress={runTest} />
    </View>
  );
}

const styles = StyleSheet.create({
  scrollView: {
    paddingHorizontal: 10,
  },
});
