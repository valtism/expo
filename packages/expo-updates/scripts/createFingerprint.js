const { getConfig } = require('@expo/config');
const Fingerprint = require('@expo/fingerprint');
const fs = require('fs');
const path = require('path');

function findUpProjectRoot(cwd) {
  if (['.', path.sep].includes(cwd)) return null;

  if (fs.existsSync(path.join(cwd, 'package.json'))) {
    return cwd;
  } else {
    return findUpProjectRoot(path.dirname(cwd));
  }
}

(async function () {
  const platform = process.argv[2];
  const possibleProjectRoot = findUpProjectRoot(process.argv[3]);
  const destinationDir = process.argv[4];

  // Remove projectRoot validation when we no longer support React Native <= 62
  let projectRoot;
  if (fs.existsSync(path.join(possibleProjectRoot, 'package.json'))) {
    projectRoot = possibleProjectRoot;
  } else if (fs.existsSync(path.join(possibleProjectRoot, '..', 'package.json'))) {
    projectRoot = path.resolve(possibleProjectRoot, '..');
  }

  process.chdir(projectRoot);

  const { exp: config } = getConfig(projectRoot, {
    isPublicConfig: true,
    skipSDKVersionRequirement: true,
  });

  const runtimeVersion = config[platform]?.runtimeVersion ?? config.runtimeVersion;
  if (typeof runtimeVersion === 'string') {
    return;
  }

  let fingerprint;
  if (runtimeVersion.policy === 'fingerprintNativeExperimental') {
    fingerprint = await Fingerprint.createProjectHashAsync(projectRoot, {
      platforms: [platform],
    });
  } else if (runtimeVersion.policy === 'fingerprintNonNativeExperimental') {
    // ignore everything in native directories to ensure fingerprint is the same
    // no matter whether project has been prebuilt
    fingerprint = await Fingerprint.createProjectHashAsync(projectRoot, {
      platforms: [platform],
      ignorePaths: ['/android/**/*', '/ios/**/*'],
    });
  } else {
    // not a policy that needs fingerprinting
    return;
  }

  fs.writeFileSync(path.join(destinationDir, 'fingerprint'), fingerprint);
})().catch((e) => {
  // Wrap in regex to make it easier for log parsers (like `@expo/xcpretty`) to find this error.
  e.message = `@build-script-error-begin\n${e.message}\n@build-script-error-end\n`;
  console.error(e);
  process.exit(1);
});
